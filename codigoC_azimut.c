#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#ifdef _WIN32
#include <windows.h>
#include <unistd.h>
#endif

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
void actualizarOrientacionPanel(struct Coordenadas coordenadas, HANDLE hSerial) {
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

    // Enviar los ángulos a Arduino
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%.2f %.2f\n", AIS_grados, Azimut_grados);
    DWORD bytesWritten;
    WriteFile(hSerial, buffer, strlen(buffer), &bytesWritten, NULL);
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
    return 1; // Es un número válido
}

// Función para obtener las coordenadas de entrada del usuario
struct Coordenadas obtenerCoordenadas() {
    struct Coordenadas coordenadas;
    char entradaLat[20], entradaLon[20];
    
    // Solicitar la latitud al usuario
    printf("Introduce la latitud del punto: ");
    fgets(entradaLat, sizeof(entradaLat), stdin);
    entradaLat[strcspn(entradaLat, "\n")] = '\0'; // Eliminar el salto de línea
    
    // Validar la entrada de la latitud
    while (!esNumero(entradaLat)) {
        printf("Entrada no válida. Introduce un número para la latitud: ");
        fgets(entradaLat, sizeof(entradaLat), stdin);
        entradaLat[strcspn(entradaLat, "\n")] = '\0'; // Eliminar el salto de línea
    }
    
    // Solicitar la longitud al usuario
    printf("Introduce la longitud del punto: ");
    fgets(entradaLon, sizeof(entradaLon), stdin);
    entradaLon[strcspn(entradaLon, "\n")] = '\0'; // Eliminar el salto de línea
    
    // Validar la entrada de la longitud
    while (!esNumero(entradaLon)) {
        printf("Entrada no válida. Introduce un número para la longitud: ");
        fgets(entradaLon, sizeof(entradaLon), stdin);
        entradaLon[strcspn(entradaLon, "\n")] = '\0'; // Eliminar el salto de línea
    }
    
    // Convertir las entradas a double y asignarlas a la estructura Coordenadas
    coordenadas.latitud = atof(entradaLat);
    coordenadas.longitud = atof(entradaLon);
    
    return coordenadas;
}

// Función principal
int main() {
    #ifdef _WIN32
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};

    // Abre el puerto serie
    hSerial = CreateFile(
        "\\\\.\\COM3", GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    );

    // Verificar si se abrió correctamente el puerto serie
    if (hSerial == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error al abrir el puerto serie\n");
        return 1;
    }

    // Configura los parámetros del puerto serie
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    // Obtener el estado actual del puerto serie
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        fprintf(stderr, "Error al obtener el estado del puerto serie\n");
        return 1;
    }

    // Establecer los parámetros del puerto serie
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    // Aplicar la configuración al puerto serie
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        fprintf(stderr, "Error al configurar el puerto serie\n");
        return 1;
    }

    // Configura los tiempos de espera del puerto serie
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    // Aplicar los tiempos de espera al puerto serie
    if (!SetCommTimeouts(hSerial, &timeouts)) {
        fprintf(stderr, "Error al configurar los tiempos de espera del puerto serie\n");
        return 1;
    }
    #endif

    // Obtener las coordenadas del usuario
    struct Coordenadas coordenadas = obtenerCoordenadas();
    printf("Latitud: %.6f\n", coordenadas.latitud);
    printf("Longitud: %.6f\n", coordenadas.longitud);
    
    // Bucle infinito para actualizar la orientación del panel solar
    while (1) {
        // Muestra la fecha y hora actual
        mostrarFechaActual();
        mostrarHoraActual();
        
        // Muestra los días transcurridos del año
        mostrarDiasTranscurridosDelAno();
        
        // Actualiza y muestra la orientación del panel solar
        actualizarOrientacionPanel(coordenadas, hSerial);

        // Espera 10 segundos antes de la siguiente actualización
        #ifdef _WIN32
        Sleep(10000);
        #endif
    }

    // Cierra el puerto serie (solo en Windows)
    #ifdef _WIN32
    CloseHandle(hSerial); // Cierra el puerto serie
    #endif

    return 0;
}
