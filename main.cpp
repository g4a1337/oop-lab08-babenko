#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

class Car {
    std::string brand_;
    int cylinders_;
    double power_;
public:
    Car(std::string brand, int cylinders, double power)
        : cylinders_(cylinders) {
        if (cylinders <= 0) throw std::invalid_argument("Invalid cylinder count");
        setBrand(brand);
        setPower(power);
    }
    virtual ~Car() = default;
    void setBrand(const std::string& brand) {
        if (brand.empty()) throw std::invalid_argument("Empty brand");
        brand_ = brand;
    }
    void setPower(double power) {
        if (!std::isfinite(power) || power <= 0)
            throw std::invalid_argument("Power must be finite and positive");
        power_ = power;
    }
    void changePower(double delta) { setPower(power_ + delta); }
    const std::string& brand() const { return brand_; }
    int cylinders() const { return cylinders_; }
    double power() const { return power_; }
    virtual const char* typeName() const { return "Car"; }
    virtual void print() const {
        std::cout << typeName() << ": brand=" << brand_ << ", cylinders="
                  << cylinders_ << ", power=" << power_ << " hp";
    }
};

class Lorry : public Car {
    double capacity_;
public:
    Lorry(std::string brand, int cylinders, double power, double capacity)
        : Car(brand,cylinders,power) { setCapacity(capacity); }
    void renameBrand(const std::string& brand) { setBrand(brand); }
    void setCapacity(double capacity) {
        if (!std::isfinite(capacity) || capacity < 0)
            throw std::invalid_argument("Capacity must be finite and nonnegative");
        capacity_ = capacity;
    }
    void changeCapacity(double delta) { setCapacity(capacity_ + delta); }
    double capacity() const { return capacity_; }
    const char* typeName() const override { return "Lorry"; }
    void print() const override {
        Car::print();
        std::cout << ", capacity=" << capacity_ << " kg";
    }
};

// Required by-value parameter and return: the Lorry part is sliced away.
Car tuneCopy(Car value) {
    value.changePower(20);
    return value;
}
// Substitution without slicing: this reference still refers to the Lorry.
void tuneOriginal(Car& value) {
    value.changePower(10);
    value.print();
    std::cout << '\n';
}

int main() {
    Car car("Toyota",4,120);
    car.setPower(130);
    car.changePower(10);
    car.print(); std::cout << '\n';
    Lorry truck("MAN",6,320,10000);
    truck.changePower(30);
    truck.renameBrand("Volvo FH");
    truck.setCapacity(12000);
    truck.changeCapacity(500);
    truck.print(); std::cout << '\n';

    Car copy = tuneCopy(truck);
    std::cout << "By value (sliced): "; copy.print(); std::cout << '\n';
    std::cout << "Original after copying: "; truck.print(); std::cout << '\n';
    bool ok = car.power() == 140 && copy.power() == 370 && truck.power() == 350
           && std::string(copy.typeName()) == "Car" && truck.capacity() == 12500;
    std::cout << "Through Car&: "; tuneOriginal(truck);
    ok = ok && truck.power() == 360 && truck.capacity() == 12500
            && truck.brand() == "Volvo FH" && truck.cylinders() == 6;

    bool invalidPower = false, invalidCapacity = false;
    try { truck.setPower(-5); }
    catch (const std::invalid_argument& e) {
        invalidPower = true; std::cout << "Rejected: " << e.what() << '\n';
    }
    try { truck.setCapacity(-1); }
    catch (const std::invalid_argument& e) {
        invalidCapacity = true; std::cout << "Rejected: " << e.what() << '\n';
    }
    ok = ok && invalidPower && invalidCapacity && truck.power() == 360
            && truck.capacity() == 12500;
    std::cout << "Self-checks: " << (ok ? "PASS" : "FAIL") << '\n';
    return ok ? 0 : 1;
}
