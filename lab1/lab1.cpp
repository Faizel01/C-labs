#include <windows.h>
#include <stdio.h>
#include <tchar.h>

// Віконна процедура для обробки повідомлень
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Інформація про пристрій: ширина поля виводу в міліметрах
        int widthMM = GetDeviceCaps(hdc, HORZSIZE);

        // Інформація про шрифт: отримуємо метрики поточного шрифту
        TEXTMETRIC tm;
        GetTextMetrics(hdc, &tm);
        int internalLeading = tm.tmInternalLeading;

        TCHAR buffer[256];
        int yPos = 30;

        // Виведення інформації у вікно програми (англійською мовою)
        TextOut(hdc, 20, yPos, TEXT("Device and Font Information:"), 29);
        yPos += 35;

        _stprintf_s(buffer, TEXT("Screen Width in mm: %d mm"), widthMM);
        TextOut(hdc, 30, yPos, buffer, (int)_tcslen(buffer));
        yPos += 30;

        _stprintf_s(buffer, TEXT("Internal Leading: %d px"), internalLeading);
        TextOut(hdc, 30, yPos, buffer, (int)_tcslen(buffer));

        EndPaint(hwnd, &ps);
        break;
    }
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
    const wchar_t CLASS_NAME[] = L"StudentClass"; // Ім'я класу вікна (ім'я студента латиницею)[cite: 3]

    WNDCLASS wc = {};
    wc.style = CS_HREDRAW; // Оновлення вікна при зміні горизонтального розміру[cite: 3]
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_HAND); // Тип іконки: IDI_HAND[cite: 3]
    wc.hCursor = LoadCursor(NULL, IDC_WAIT); // Тип курсору: IDC_WAIT[cite: 3]
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); // Колір фону вікна: світло-сірий[cite: 3]
    wc.lpszClassName = CLASS_NAME;

    // Реєстрація класу вікна в системі
    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error"), MB_ICONERROR);
        return 0;
    }

    // Задана ширина та висота вікна
    int windowWidth = 400; //[cite: 3]
    int windowHeight = 250; //[cite: 3]

    // Обчислення позиції для центру екрана
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    // Створення вікна програми
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        TEXT("Lab Work - Variant 2"), // Заголовок вікна
        WS_OVERLAPPEDWINDOW | WS_MINIMIZEBOX, // Стиль: WS_OVERLAPPEDWINDOW та кнопка мінімізації[cite: 3]
        posX, posY, // Позиція вікна в центрі екрана[cite: 3]
        windowWidth, windowHeight, // Ширина = 400, висота = 250 точок[cite: 3]
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    // Відображення та оновлення вікна
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Цикл обробки повідомлень операційної системи
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}