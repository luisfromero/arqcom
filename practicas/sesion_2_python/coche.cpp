#include <iostream>
#include <string>

class Coche {
protected:
    std::string marca;
    std::string modelo;
    int velocidad;

public:
    Coche(const std::string& marca, const std::string& modelo)
        : marca(marca), modelo(modelo), velocidad(0) {}

    void acelerar(int incremento) {
        velocidad += incremento;
    }

    void frenar(int decremento) {
        velocidad -= decremento;
        if (velocidad < 0) {
            velocidad = 0;
        }
    }

    void mostrar() const {
        std::cout << marca << " " << modelo
                  << " | velocidad: " << velocidad << " km/h" << std::endl;
    }
};

class Mercedes : public Coche {
    std::string clase;

public:
    Mercedes(const std::string& modelo, const std::string& clase)
        : Coche("Mercedes", modelo), clase(clase) {}

    void mostrar() const {
        std::cout << marca << " " << modelo
                  << " | clase: " << clase
                  << " | velocidad: " << velocidad << " km/h" << std::endl;
    }
};

int main() {
    Coche coche1("Seat", "Ibiza");
    coche1.acelerar(50);
    coche1.mostrar();

    Mercedes mercedes1("Clase A", "Compacto");
    mercedes1.acelerar(80);
    mercedes1.mostrar();

    Mercedes mercedes2("Clase C", "Berlina");
    mercedes2.acelerar(120);
    mercedes2.frenar(20);
    mercedes2.mostrar();

    return 0;
}
