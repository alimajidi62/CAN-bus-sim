export module VariantTest;
import <iostream>;
import <variant>;
import <vector>;
import <string>;

export void TestVarient() {
    using MatrixElement = std::variant<bool, int, std::string>;
    using Row = std::vector<MatrixElement>;
    using Matrix = std::vector<Row>;
    MatrixElement s;
    s = 1;
    s = "ss";
    Matrix matrix = {
        {true, false, 100, "2000"},
        {false, 10, true, 100},
        {100000, true, 1, false}
    };
    for (const auto& row : matrix) {
        for (const auto& elem : row) {
            std::visit([](auto&& arg) { std::cout << arg << " "; }, elem);
        }
        std::cout << std::endl;
    }
}
