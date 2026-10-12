#include <windows.h>
#include <stdio.h>
#include <time.h>

// Глобальна структура для зберігання часу
typedef struct {
    int hour;
    int minute;
    int second;
} SystemTimeData;

SystemTimeData g_timeData = { 0, 0, 0 };

// Об'єкт критичної секції для безпечного синхронізованого доступу
CRITICAL_SECTION g_csTime;

// Прапорець для коректного завершення роботи потоків
volatile bool g_isRunning = true;

// Перший потік: періодично читає системний час
DWORD WINAPI TimeReaderThread(LPVOID lpParam)
{
    while (g_isRunning)
    {
        time_t rawTime;
        struct tm timeInfo;
        time(&rawTime);
        localtime_s(&timeInfo, &rawTime);

        // Захищаємо запис у спільну структуру критичною секцією
        EnterCriticalSection(&g_csTime);
        g_timeData.hour = timeInfo.tm_hour;
        g_timeData.minute = timeInfo.tm_min;
        g_timeData.second = timeInfo.tm_sec;
        LeaveCriticalSection(&g_csTime);

        // Оновлюємо кожну секунду
        Sleep(1000);
    }
    return 0;
}

// Другий потік: виводить дані структури на екран
DWORD WINAPI TimePrinterThread(LPVOID lpParam)
{
    while (g_isRunning)
    {
        SystemTimeData localData;

        // Захищаємо читання спільної структури критичною секцією
        EnterCriticalSection(&g_csTime);
        localData = g_timeData;
        LeaveCriticalSection(&g_csTime);

        // Виведення даних на екран (у консоль)
        printf("Current Time from Structure: %02d:%02d:%02d\n",
            localData.hour, localData.minute, localData.second);

        // Виводимо оновлення раз на секунду
        Sleep(1000);
    }
    return 0;
}

// Головна функція програми
int main()
{
    // Встановлюємо коректну кодировку для консолі
    SetConsoleOutputCP(65001);

    printf("Multithreading\n");
    printf("Press 'Enter' to stop the program.\n\n");

    // Ініціалізація критичної секції
    InitializeCriticalSection(&g_csTime);

    // Створення першого потоку (Читач часу)
    HANDLE hThread1 = CreateThread(
        NULL, // Атрибути безпеки за замовчуванням
        0, // Розмір стека за замовчуванням
        TimeReaderThread, // Функція потоку
        NULL, // Параметр передачі
        0, // Прапорці створення (потік запускається одразу)
        NULL // Ідентифікатор потоку
    );

    // Створення другого потоку (Вивід на екран)
    HANDLE hThread2 = CreateThread(
        NULL,
        0,
        TimePrinterThread,
        NULL,
        0,
        NULL
    );

    if (hThread1 == NULL || hThread2 == NULL)
    {
        printf("Error: Failed to create threads!\n");
        DeleteCriticalSection(&g_csTime);
        return 1;
    }

    // Очікування натискання клавіші Enter для завершення програми
    getchar();

    // Сигнал потокам про зупинку
    g_isRunning = false;

    // Очікуємо завершення обох потоків
    HANDLE threads[2] = { hThread1, hThread2 };
    WaitForMultipleObjects(2, threads, TRUE, INFINITE);

    // Закриття дескрипторів потоків
    CloseHandle(hThread1);
    CloseHandle(hThread2);

    // Звільнення ресурсів критичної секції
    DeleteCriticalSection(&g_csTime);

    printf("\nProgram safely terminated.\n");
    return 0;
}