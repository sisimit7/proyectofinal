#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

// Prototipos
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void AddControls(HWND);
void InterpolateAndDisplay(HWND);

// Variables globales para los controles
HWND hInputX, hInputY, hInputResult;
HWND hButton;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    WNDCLASSW wc = {0};

    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"myWindowClass";
    wc.lpfnWndProc = WindowProcedure;

    if (!RegisterClassW(&wc)) {
        return -1;
    }

    CreateWindowW(L"myWindowClass", L"Interpolación de Newton", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
                  100, 100, 500, 500, NULL, NULL, NULL, NULL);

    MSG msg = {0};

    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_COMMAND:
            if (wp == 1) {
                InterpolateAndDisplay(hWnd);
            }
            break;
        case WM_CREATE:
            AddControls(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProcW(hWnd, msg, wp, lp);
    }
    return 0;
}

void AddControls(HWND hWnd) {
    CreateWindowW(L"static", L"Valores de X (separados por comas):", WS_VISIBLE | WS_CHILD | SS_CENTER, 
                  50, 50, 400, 25, hWnd, NULL, NULL, NULL);
    hInputX = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 
                            50, 80, 400, 25, hWnd, NULL, NULL, NULL);

    CreateWindowW(L"static", L"Valores de Y (separados por comas):", WS_VISIBLE | WS_CHILD | SS_CENTER, 
                  50, 110, 400, 25, hWnd, NULL, NULL, NULL);
    hInputY = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 
                            50, 140, 400, 25, hWnd, NULL, NULL, NULL);

    hButton = CreateWindowW(L"button", L"Calcular Interpolación", WS_VISIBLE | WS_CHILD | WS_BORDER, 
                            150, 180, 200, 50, hWnd, (HMENU)1, NULL, NULL);

    hInputResult = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY, 
                                 50, 250, 400, 200, hWnd, NULL, NULL, NULL);
}

std::vector<double> ParseInput(HWND hWnd) {
    wchar_t buffer[1024];
    GetWindowTextW(hWnd, buffer, 1024);
    std::wstring input = buffer;
    std::wistringstream iss(input);
    std::vector<double> values;
    std::wstring token;

    while (std::getline(iss, token, L',')) {
        values.push_back(std::stod(token));
    }
    return values;
}

void InterpolateAndDisplay(HWND hWnd) {
    std::vector<double> xValues = ParseInput(hInputX);
    std::vector<double> yValues = ParseInput(hInputY);

    if (xValues.size() != yValues.size() || xValues.empty()) {
        SetWindowTextW(hInputResult, L"Los valores de X e Y deben tener la misma longitud y no pueden estar vacíos.");
        return;
    }

    int n = xValues.size();
    std::vector<std::vector<double>> diffTable(n, std::vector<double>(n, 0));

    for (int i = 0; i < n; ++i) {
        diffTable[i][0] = yValues[i];
    }

    for (int j = 1; j < n; ++j) {
        for (int i = 0; i < n - j; ++i) {
            diffTable[i][j] = (diffTable[i + 1][j - 1] - diffTable[i][j - 1]) / (xValues[i + j] - xValues[i]);
        }
    }

    std::wstring result = L"Diferencias divididas de Newton:\r\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n - i; ++j) {
            result += std::to_wstring(diffTable[i][j]) + L" ";
        }
        result += L"\r\n";
    }

    SetWindowTextW(hInputResult, result.c_str());
}