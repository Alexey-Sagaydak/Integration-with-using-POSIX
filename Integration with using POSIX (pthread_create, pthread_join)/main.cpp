#include <chrono>
#include <cmath>
#include <iostream>
#include "main.h"
#include "pthread.h"

#pragma comment(lib, "pthreadVCE2.lib")

struct integration_params {
    double start_point, end_point;
    double step;
    double sum;
};

int main() {
    setlocale(LC_ALL, "Rus");
    run_menu();
    return 0;
}

void* calculate_integral_part(void* threadid) {
    double temp_sum = 0;

    integration_params* buff = (integration_params*)threadid;
    double step = buff->step;

    for (double x = buff->start_point; x < buff->end_point - 0.00000001; x += step * 3)
        temp_sum += math_function(x) + 3 * math_function(x + step) + 3 * math_function(x + step * 2) + math_function(x + step * 3);

    ((integration_params*)threadid)->sum = temp_sum;

    return nullptr;
}

double calculate_full_integral(double left, double right, double step, int threads_count) {
    double temp, integral_sum = 0;

    pthread_t* threads = new pthread_t[threads_count];
    integration_params* data = new integration_params[threads_count];

    temp = left;
    for (int i = 0; i < threads_count; i++) {
        data[i].start_point = temp;
        temp += (right - left) / threads_count;
        data[i].end_point = temp;
        data[i].step = step;
    }

    for (int i = 0; i < threads_count; i++) {
        if (pthread_create(&threads[i], NULL, calculate_integral_part, &data[i])) {
            std::cout << "Ошибка: не удалось создать поток\n";
            exit(-1);
        }
    }

    for (int i = 0; i < threads_count; i++) {
        pthread_join(threads[i], NULL);
        integral_sum += data[i].sum;
    }

    integral_sum *= 3.0 * step / 8.0;

    delete[] threads;
    delete[] data;

    return integral_sum;
}

double math_function(double x) {
    return log(x);
}

void run_menu() {
    time_t begin = time(NULL);
    std::chrono::steady_clock::time_point start, end;

    double left, right, step, integral_sum;
    int step_count, threads_count;
    const int max_threads_value = 8;

    std::cout << "Интегрирование методом Ньютона-Котеса 3-го порядка\n";
    std::cout << "Введите интервал интегрирования:\n";

    std::cout << "Левая граница: "; std::cin >> left;
    std::cout << "Правая граница: "; std::cin >> right;

    while (right <= left) {
        std::cout << "Правая граница должна быть больше левой!\n";
        std::cout << "Левая граница: "; std::cin >> left;
        std::cout << "Правая граница: "; std::cin >> right;
    }

    std::cout << "Введите количество шагов (кратное 3): "; std::cin >> step_count;
    while (step_count % 3 != 0) {
        std::cout << "Количество шагов должно быть кратно 3\n";
        std::cout << "Введите количество шагов (кратное 3): "; std::cin >> step_count;
    }

    step = (right - left) / step_count;

    std::cout << "Введите количество потоков (от 1 до " << max_threads_value << "): "; std::cin >> threads_count;

    while ((threads_count > max_threads_value) || (threads_count < 1)) {
        std::cout << "Количество потоков должно быть от 1 до " << max_threads_value;
        std::cout << "Введите количество потоков (от 1 до " << max_threads_value << "): "; std::cin >> threads_count;
    }

    start = std::chrono::steady_clock::now();

    integral_sum = calculate_full_integral(left, right, step, threads_count);

    end = std::chrono::steady_clock::now();

    std::cout << "Результат: " << integral_sum << " " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " мс";
}
