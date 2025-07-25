#include <Windows.h>
#include <gl/GL.h>
#include <chrono>
#include <fstream>
#include <thread>
#include <random>
#include <cmath>

#include "settings.h"
#include "trayUtils.h"
#include "DesktopUtils.h"
#include "star.h"
#include "rendering.h"
#include "triangle.h"

#define TAU_F 6.2831853f

constinit bool running = true;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_TRAYICON) {
		if (lParam == WM_RBUTTONUP) {
			// Create a popup menu
			HMENU menu = CreatePopupMenu();
			AppendMenuW(menu, MF_STRING, 1, L"Quit");

			// Get the cursor position
			POINT cursorPos;
			GetCursorPos(&cursorPos);

			// Show the menu
			SetForegroundWindow(hwnd);
			// Example with TPM_NONOTIFY to avoid blocking
			int selection = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY, cursorPos.x - 120, cursorPos.y - 22, 0, hwnd, nullptr);
			DestroyMenu(menu);

			// Handle the menu selection
			if (selection == 1) {
				OnQuit(&hwnd, &running);
			}
		}
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static float randomUniform(float start, float end) {
	// Create a random device and a random engine
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(start, end); // Range [start, end)

	return static_cast<float>(dist(gen)); // Generate the random number
}

static inline void checkEvents(MSG& msg, bool& running) {
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			running = false;
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

static inline void gameTick(float& frameTime, const float& stepInterval, float& fractionalTime) {
	if (frameTime < stepInterval) {
		// Calculate total sleep time including any leftover fractional time
		float totalSleepTime = (stepInterval - frameTime) + fractionalTime;

		// Truncate to whole milliseconds
		int sleepMilliseconds = static_cast<int>(totalSleepTime * 1e+3f);

		// Calculate remaining fractional time and ensure it�s within 0.0f to 1.0f
		fractionalTime = (totalSleepTime - sleepMilliseconds * 1e-3f);

		// Sleep for the calculated milliseconds
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliseconds));
	}
}

void (Star::* Star::renderFunc)(const int) const = &Star::realRender;

static std::array<float, 4> interpolateColors(const std::vector<std::array<float, 4>>& colors, float t) {
	if (colors.empty()) return { 0, 0, 0, 1 }; // Default: Black
	if (colors.size() == 1) return colors[0]; // Only 1 color

	// Clamp t to [0, 1]
	t = max(0.0f, min(1.0f, t));

	// Calculate segment size (N colors → N-1 segments)
	float segmentSize = 1.0f / (colors.size() - 1);

	// Find the two closest colors
	int index = static_cast<int>(t / segmentSize);
	index = min(index, static_cast<int>(colors.size()) - 2); // Prevent overflow

	// Compute local interpolation weight (0 ≤ local_t ≤ 1)
	float local_t = (t - index * segmentSize) / segmentSize;

	// Get the two colors to blend
	const auto& c1 = colors[index];
	const auto& c2 = colors[index + 1];

	// Linear interpolation (lerp) for each channel
	float r = std::get<0>(c1) + (std::get<0>(c2) - std::get<0>(c1)) * local_t;
	float g = std::get<1>(c1) + (std::get<1>(c2) - std::get<1>(c1)) * local_t;
	float b = std::get<2>(c1) + (std::get<2>(c2) - std::get<2>(c1)) * local_t;
	float a = std::get<3>(c1) + (std::get<3>(c2) - std::get<3>(c1)) * local_t;

	return { r, g, b, a };
}


int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	const wchar_t* className = L"EclipseFrameClass";
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = className;
	wc.hIconSm = nullptr;

	RegisterClassEx(&wc);

	const int Width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	const int Height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	const int leftMost = GetSystemMetrics(SM_XVIRTUALSCREEN);
	const int topMost = GetSystemMetrics(SM_YVIRTUALSCREEN);

	HWND hwnd = CreateWindowEx(
		WS_EX_TOOLWINDOW,
		className,
		L"Eclipse Frame",
		WS_POPUP,
		0, 0,
		Width, Height,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hwnd) return -1;

	const Settings settings = loadSettings("settings.json");

	const float stepInterval = 1.0f / settings.targetFPS;

	if (settings.stars.draw) {
		Star::renderFunc = &Star::realRender;
	}
	else {
		Star::renderFunc = &Star::emptyRender;
	}

	// Load the icon from resources
	HICON hIcon = LoadIconFromResource();

	// Add the tray icon
	AddTrayIcon(&hwnd, &hIcon, L"Just a Simple Icon");

	wchar_t* originalWallpaper = GetCurrentWallpaper();

	HDC hdc = GetDC(hwnd); // Get device context

	// Step 1: Set up the pixel format
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);

	// Step 2: Create and set the rendering context
	HGLRC hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);

	// OpenGL is now set up for this window!

	float dt{0};
	float frameTime{0};
	float fractionalTime{0};

	POINT mousePos;
	
	Star* stars = new Star[settings.stars.count];

	const float offsetBounds = settings.offsetBounds;
	const float roffsetBounds = -settings.offsetBounds;
	const float woffsetBounds = offsetBounds + Width;
	const float hoffsetBounds = offsetBounds + Height;

	triangulateio in, out, vorout;
	// Initialize structs (zero them out)
	memset(&in, 0, sizeof(in));
	memset(&out, 0, sizeof(out));
	memset(&vorout, 0, sizeof(vorout));

	in.numberofpoints = settings.stars.count;
	in.pointlist = (double*)malloc(sizeof(double) * in.numberofpoints * 2);

	// Fill the array with Star objects
	for (int i = 0; i < settings.stars.count; ++i) {
		Star& star = stars[i];
		float x = randomUniform(roffsetBounds, woffsetBounds);
		float y = randomUniform(roffsetBounds, woffsetBounds);
		float speed = randomUniform(settings.stars.minSpeed, settings.stars.maxSpeed);
		float angle = randomUniform(0, TAU_F);
		star.x = x;
		star.y = y;
		star.orgx = x;
		star.orgy = y;
		star.speedx = cosf(angle)*speed;
		star.speedy = sinf(angle)*speed;
		star.radius = settings.stars.radius;
		star.color = settings.stars.color;

		in.pointlist[i * 2] = x;
		in.pointlist[i * 2 + 1] = y;
	}

	char* mode = (char*)"zQ";
	triangulate(mode, &in, &out, &vorout);  // 'zQ' = no output files, quiet mode

	auto newF = std::chrono::high_resolution_clock::now();
	auto oldF = std::chrono::high_resolution_clock::now();
	auto endF = std::chrono::high_resolution_clock::now();

	SetAsDesktop(hwnd);

	ShowWindow(hwnd, SW_SHOW);

	// Message loop
	MSG msg = {};
	while (running) {
		// get deltt time
		oldF = newF;
		newF = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float>(newF - oldF).count();

		// Check for messages
		checkEvents(msg, running);

		// draw background
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glOrtho(0, Width, Height, 0, -1, 1);

		GetCursorPos(&mousePos);
		mousePos.x -= leftMost;
		mousePos.y -= topMost;

		for (int starIdx = 0; starIdx < settings.stars.count; ++starIdx) {
			Star& star = stars[starIdx];
			star.move(dt);

			star.render(settings.stars.nSegments);

			if (star.orgx < roffsetBounds) {
				star.speedx = std::abs(star.speedx);
			}
			else if (star.orgx > woffsetBounds) {
				star.speedx = -std::abs(star.speedx);
			}

			if (star.orgy < roffsetBounds) {
				star.speedy = std::abs(star.speedy);
			}
			else if (star.orgy > hoffsetBounds) {
				star.speedy = -std::abs(star.speedy);
			}

			in.pointlist[starIdx * 2] = star.x;
			in.pointlist[starIdx * 2 + 1] = star.y;
		}

		triangulate(mode, &in, &out, &vorout);

		// Extract triangles
		for (int i = 0; i < out.numberoftriangles; i++) {
			int a = out.trianglelist[i * 3 + 0];
			int b = out.trianglelist[i * 3 + 1];
			int c = out.trianglelist[i * 3 + 2];

			// Get coordinates of each point
			float x1 = static_cast<float>(out.pointlist[a * 2 + 0]);
			float y1 = static_cast<float>(out.pointlist[a * 2 + 1]);
			float x2 = static_cast<float>(out.pointlist[b * 2 + 0]);
			float y2 = static_cast<float>(out.pointlist[b * 2 + 1]);
			float x3 = static_cast<float>(out.pointlist[c * 2 + 0]);
			float y3 = static_cast<float>(out.pointlist[c * 2 + 1]);

			float cy = (y1 + y2 + y3) / 3;
			std::array<float, 4> color = interpolateColors(settings.backGroundColors, cy / hoffsetBounds);

			filledTriangle(x1, y1, x2, y2, x3, y3, color[0], color[1], color[2], color[3]);
		}


		SwapBuffers(hdc);

		endF = std::chrono::high_resolution_clock::now();
		frameTime = std::chrono::duration<float>(endF - newF).count();

		gameTick(frameTime, stepInterval, fractionalTime);
	}

	SetParent(hwnd, nullptr);
	SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)originalWallpaper, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(hglrc);
	ReleaseDC(hwnd, hdc);
	
	DestroyWindow(hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	free(in.pointlist);
	free(out.pointlist);
	free(out.trianglelist);

	delete[] originalWallpaper;
	delete[] stars;
	
	DestroyIcon(hIcon);

	return 0;
}
