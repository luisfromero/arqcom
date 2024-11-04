# Guía para Instalar Android Studio y Crear una Aplicación Kotlin con Jetpack Compose

## 1. Requisitos Previos

Asegúrate de tener los siguientes requisitos antes de comenzar:

- **Sistema Operativo**: Windows, macOS o Linux.
- **Java Development Kit (JDK)**: Android Studio incluye un JDK, pero asegúrate de que esté instalado y configurado si necesitas una versión específica.

## 2. Instalación de Android Studio

### Windows y macOS

1. **Descargar Android Studio**:
   - Ve a la [página de descarga de Android Studio](https://developer.android.com/studio).
   - Selecciona la versión adecuada para tu sistema operativo y descarga el instalador.

2. **Instalar Android Studio**:
   - Ejecuta el instalador y sigue las instrucciones en pantalla.
   - Acepta los términos de servicio y selecciona las opciones recomendadas.

3. **Configurar Android Studio**:
   - Al abrir Android Studio por primera vez, se te pedirá que configures algunos ajustes iniciales. Acepta las configuraciones predeterminadas.

### Linux

1. **Descargar Android Studio**:
   - Abre una terminal y usa el siguiente comando para descargar el archivo `.zip`:
     ```bash
     wget https://r2.download.jetbrains.com/androidstudio/ide-zips/2024.1.1/android-studio-2024.1.1-linux.tar.gz
     ```

2. **Descomprimir y mover Android Studio**:
   ```bash
   sudo tar -xvzf android-studio-2024.1.1-linux.tar.gz -C /opt/
