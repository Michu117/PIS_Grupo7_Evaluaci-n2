#include <stdio.h>
#include <time.h>
#include <math.h>
#include <windows.h>


#define PI 3.14159265

// Definición de la estructura Coordenadas
struct Coordenadas {
    double latitud;
    double longitud;
};

// Función para mostrar los días transcurridos del año
void mostrarDiasTranscurridosDelAno() {
    time_t tiempo_actual;
    struct tm *tiempo_local;

    tiempo_actual = time(NULL); // Obtener el tiempo actual
    tiempo_local = localtime(&tiempo_actual); // Convertir el tiempo a la estructura tm

    printf("Días transcurridos del año: %d\n", tiempo_local->tm_yday + 1); // Mostrar los días transcurridos del año
}

// Función para mostrar la fecha actual
void mostrarFechaActual() {
    time_t tiempo_actual;
    struct tm *tiempo_local;

    tiempo_actual = time(NULL); // Obtener el tiempo actual
    tiempo_local = localtime(&tiempo_actual); // Convertir el tiempo a la estructura tm

    printf("La fecha actual es: %02d-%02d-%04d\n", // Mostrar la fecha actual en formato dd-mm-aaaa
           tiempo_local->tm_mday,
           tiempo_local->tm_mon + 1,
           tiempo_local->tm_year + 1900);
}

// Función para mostrar la hora actual
void mostrarHoraActual() {
    time_t tiempo_actual;
    struct tm *tiempo_local;

    tiempo_actual = time(NULL); // Obtener el tiempo actual
    tiempo_local = localtime(&tiempo_actual); // Convertir el tiempo a la estructura tm

    printf("La hora actual es: %02d:%02d:%02d\n", // Mostrar la hora actual en formato hh:mm:ss
           tiempo_local->tm_hour,
           tiempo_local->tm_min,
           tiempo_local->tm_sec);
}

// Función para calcular la declinación solar
double calcularDeclinacionSolar(int diasTranscurridos) {
    double termino2radian, declinacionSolar;

    // Calcular el término en radianes
    termino2radian = ((360.0 / 365.0) * (diasTranscurridos + 10)) * (PI / 180.0);

    // Calcular la declinación solar
    declinacionSolar = -23.44 * cos(termino2radian);

    return declinacionSolar; // Devolver la declinación solar en grados
}

// Función para calcular la ecuación del tiempo
double calcularEcuacionDelTiempo(int diasTranscurridos) {
    double BAH, EoT;

    // Calcular el término BAH en radianes
    BAH = ((360.0 / 365.0) * (diasTranscurridos - 81)) * (PI / 180.0);

    // Calcular la Ecuación del Tiempo
    EoT = 9.87 * sin(2 * BAH) - 7.53 * cos(BAH) - 1.5 * sin(BAH);

    return EoT; // Devolver la ecuación del tiempo en minutos
}

// Función para calcular el Tiempo Solar Verdadero
double calcularTiempoSolarVerdadero(double horaDecimal, double longitudPunto, double ecuacionTiempo) {
    int zonaHo = -5; // Zona horaria
    double longitudEstandar, TSV;

    // Calcular la longitud estándar en grados
    longitudEstandar = zonaHo * 15.0;

    // Calcular el Tiempo Solar Verdadero
    TSV = horaDecimal + (longitudPunto - longitudEstandar) / 15.0 + ecuacionTiempo / 60.0;

    return TSV; // Devolver el Tiempo Solar Verdadero en horas
}

// Función para calcular el ángulo horario
double calcularAnguloHorario(double tiempoSolarVerdadero) {
    double AHR = (tiempoSolarVerdadero - 12) * 15 * PI / 180;
    return AHR; // Devolver el ángulo horario en radianes
}

// Función para convertir grados a radianes
double convertirARadianes(double grados) {
    return grados * PI / 180.0; // Convertir grados a radianes
}

// Función para convertir radianes a grados
double convertirAGrados(double radianes) {
    return radianes * 180 / PI; // Convertir radianes a grados
}

// Función para calcular el Ángulo de Inclinación Solar
double calcularAnguloInclinacionSolar(double declinacionSolarRad, double LatitudPuntoRad, double AHR) {
    double AIS_rad = asin(sin(declinacionSolarRad) * sin(LatitudPuntoRad) + cos(declinacionSolarRad) * cos(LatitudPuntoRad) * cos(AHR));
    double AIS_grados = convertirAGrados(AIS_rad);
    return AIS_grados; // Devolver el Ángulo de Inclinación Solar en grados
}

// Función para calcular el Azimut Solar
double calcularAzimutSolar(double declinacionSolarRad, double AIS_rad, double LatitudPuntoRad, double AHR) {
    double Azimut_rad;
    // Calcular el Azimut solar en radianes
    Azimut_rad = acos((sin(declinacionSolarRad) * cos(LatitudPuntoRad) - cos(declinacionSolarRad) * sin(LatitudPuntoRad) * cos(AHR)) / cos(AIS_rad));
    // Ajustar el Azimut basado en el ángulo horario
    if (sin(AHR) > 0) {
        Azimut_rad = 2 * PI - Azimut_rad;
    }
    // Convertir a grados
    double Azimut_grados = convertirAGrados(Azimut_rad);
    // Asegurar que el azimut esté en el rango [0, 360]
    if (Azimut_grados < 0) {
        Azimut_grados += 360;
    } else if (Azimut_grados >= 360) {
        Azimut_grados -= 360;
    }
    return Azimut_grados; // Devolver el Azimut Solar en grados
}

// Función para actualizar y mostrar la orientación del panel solar
void actualizarOrientacionPanel(struct Coordenadas coordenadas) {
    int diasTranscurridos;
    double declinacion, ecuacionTiempo, tiempoSolarVerdadero, AHR, AIS_grados, Azimut_grados, declinacionSolarRad, LatitudPuntoRad;

    // Obtener el número de días transcurridos del año
    time_t tiempo_actual = time(NULL);
    struct tm *tiempo_local = localtime(&tiempo_actual);
    diasTranscurridos = tiempo_local->tm_yday + 1;

    // Calcular la declinación solar
    declinacion = calcularDeclinacionSolar(diasTranscurridos);

    // Calcular la Ecuación del Tiempo
    ecuacionTiempo = calcularEcuacionDelTiempo(diasTranscurridos);

    // Calcular el Tiempo Solar Verdadero
    double horaDecimal = tiempo_local->tm_hour + tiempo_local->tm_min / 60.0 + tiempo_local->tm_sec / 3600.0;
    tiempoSolarVerdadero = calcularTiempoSolarVerdadero(horaDecimal, coordenadas.longitud, ecuacionTiempo);

    // Calcular el Ángulo Horario en Radianes
    AHR = calcularAnguloHorario(tiempoSolarVerdadero);

    // Convertir declinación solar y latitud del punto a radianes
    declinacionSolarRad = convertirARadianes(declinacion);
    LatitudPuntoRad = convertirARadianes(coordenadas.latitud);

    // Calcular el Ángulo de Inclinación Solar
    AIS_grados = calcularAnguloInclinacionSolar(declinacionSolarRad, LatitudPuntoRad, AHR);

    // Calcular el Azimut Solar
    double AIS_rad = convertirARadianes(AIS_grados);
    Azimut_grados = calcularAzimutSolar(declinacionSolarRad, AIS_rad, LatitudPuntoRad, AHR);

    // Mostrar la orientación del panel solar
    printf("Orientación del panel solar:\n");
    printf("  Ángulo de Inclinación Solar: %.2f grados\n", AIS_grados);
    printf("  Azimut Solar: %.2f grados\n", Azimut_grados);
}

// Función para verificar si la entrada es un número
int esNumero(char *entrada) {
    // Verificar cada caracter de la cadena
    while (*entrada) {
        if (!isdigit(*entrada) && *entrada != '.' && *entrada != '-') {
            return 0;  // No es un número válido
        }
        entrada++;
    }
    return 1;  // Es un número válido
}

int main() {
    struct Coordenadas coordenadas;

    // Variables temporales para almacenar la entrada del usuario
    char buffer[100];
    // Solicitar al usuario que ingrese la longitud del lugar
    while (1) {
        printf("Ingrese la longitud del lugar (ejemplo: -79.20422): ");
        scanf("%s", buffer);

        // Verificar si la entrada es un número válido
        if (esNumero(buffer)) {
            sscanf(buffer, "%lf", &coordenadas.longitud);
            break;  // Salir del bucle si es un número válido
        } else {
            printf("Error: Ingrese un valor numérico válido.\n");
        }
    }

    // Solicitar al usuario que ingrese la latitud del lugar
    while (1) {
        printf("Ingrese la latitud del lugar (ejemplo: -3.99313): ");
        scanf("%s", buffer);

        // Verificar si la entrada es un número válido
        if (esNumero(buffer)) {
            sscanf(buffer, "%lf", &coordenadas.latitud);
            break;  // Salir del bucle si es un número válido
        } else {
            printf("Error: Ingrese un valor numérico válido.\n");
        }
    }

    // Bucle para actualizar la orientación del panel solar continuamente
    while (1) { 
        actualizarOrientacionPanel(coordenadas);
        // Esperar un minuto antes de la siguiente actualización (simulado con sleep)
        Sleep(60000);
    }

    return 0;
}
