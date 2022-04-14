#include <fmt/chrono.h>

int main() {
	using namespace std::literals::chrono_literals;
	fmt::print("Default format: {} {}\n", 42s, 100ms);
	fmt::print("strftime-like format: {:%H:%M:%S}\n", 3h + 15min + 30s);
}