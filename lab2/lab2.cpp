#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <tchar.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Віконна процедура для обробки повідомлень
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Отримуємо поточні розміри клієнтської області вікна
        RECT rect;
        GetClientRect(hwnd, &rect);
        int clientWidth = rect.right - rect.left;
        int clientHeight = rect.bottom - rect.top;

        // Центр вікна
        int cx = clientWidth / 2;
        int cy = clientHeight / 2;

        // Базові розміри вікна для розрахунку коефіцієнта пропорційного масштабування
        float baseWidth = 600.0f;
        float baseHeght = 450.0f;

        // Коефіцієнт масштабу (зберігає пропорції)
        float scaleX = (float)clientWidth / baseWidth;
        float scaleY = (float)clientHeight / baseHeght;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        if (scale < 0.1f) scale = 0.1f; // Захист від занадто малого розміру

        // Базові радіуси еллипса (діагоналі 250 і 100, тобто півосі 125 і 50)
        float baseA = 125.0f;
        float baseB = 50.0f;
        float a = baseA * scale;
        float b = baseB * scale;

        // Малюємо заповнений еліпс у центрі вікна
        HBRUSH hEllipseBrush = CreateSolidBrush(RGB(100, 150, 255)); // Блакитний пензель
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hEllipseBrush);
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 128));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        Ellipse(hdc, cx - (int)a, cy - (int)b, cx + (int)a, cy + (int)b);

        // Розрахунок та побудова описаного п'ятикутника навколо еліпса
        // Сторони дотикаються до еліпса у точках з кутами alpha = 2*pi*k / 5
        POINT pts[5];
        for (int k = 0; k < 5; k++)
        {
            double alpha1 = (2.0 * M_PI * k) / 5.0;
            double alpha2 = (2.0 * M_PI * (k + 1)) / 5.0;

            // Рівняння дотичних до еліпса дозволяють знайти вершини багатокутника (перетин дотичних)
            double A1 = cos(alpha1) / a;
            double B1 = sin(alpha1) / b;
            double A2 = cos(alpha2) / a;
            double B2 = sin(alpha2) / b;

            double D = A1 * B2 - A2 * B1;
            double Dx = B2 - B1;
            double Dy = A1 - A2;

            double x = Dx / D;
            double y = Dy / D;

            pts[k].x = cx + (LONG)round(x);
            pts[k].y = cy + (LONG)round(y);
        }

        // Налаштування пензля для контуру п'ятикутника (без заливки внутрішньої області)
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        HPEN hPolygonPen = CreatePen(PS_SOLID, 3, RGB(200, 50, 50)); // Червоний контур
        SelectObject(hdc, hPolygonPen);

        // Малюємо багатокутник
        Polygon(hdc, pts, 5);

        // Очищення ресурсів GDI
        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(hEllipseBrush);
        DeleteObject(hPen);
        DeleteObject(hPolygonPen);

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_SIZE:
        // При зміні розміру вікна примусово оновлюємо (перемальовуємо) вміст
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Головна функція програми
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"StudentClass"; // Ім'я класу вікна

    WNDCLASS wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW; // Перемальовування при зміні розмірів по горизонталі та вертикалі
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Білий фон вікна
    wc.lpszClassName = CLASS_NAME;

    // Реєстрація класу вікна
    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, TEXT("Помилка реєстрації класу вікна!"), TEXT("Помилка"), MB_ICONERROR);
        return 0;
    }

    // Задані розміри вікна
    int windowWidth = 600; //[cite: 5]
    int windowHeight = 450; //[cite: 5]

    // Центрування вікна на екрані
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    // Створення вікна додатку
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        TEXT("Лабораторна робота - GDI"),
        WS_OVERLAPPEDWINDOW,
        posX, posY,
        windowWidth, windowHeight,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Цикл обробки повідомлень
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}