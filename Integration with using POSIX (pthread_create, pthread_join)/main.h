#pragma once

struct integration_params;

void* calculate_integral_part(void* threadid);
double calculate_full_integral(double left, double right, double step, int threads_count);
double math_function(double x);
void run_menu();
