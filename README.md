## Arquitectura de computadores. Escuela de Industriales. Curso 2026/27

El propósito de este repositorio es, principalmente, el almacenamiento de códigos, notebooks y archivos utilizados en la asignatura de Arquitectura de Computadores, de forma que exista una mayor flexibilidad que la que ofrece el [Campus Virtual](https://eii.cv.uma.es/course/view.php?id=193).

### Prerrequisitos


1. [Introducción a Git](#introducción-a-git)
2. [Introducción a Markdown](#introducción-a-markdown)
3. [Instalación de VSCode](#instalación-de-vscode)
4. [Práctica 1. Primeros pasos de compilación](http://casium.uma.es/arqcom-1)

<br>
<br>
## Introducción a Git


### ¿Qué es Git?

Git es un sistema de control de versiones distribuido, diseñado para manejar todo tipo de proyectos, grandes o pequeños, con rapidez y eficiencia. Fue creado por Linus Torvalds en 2005, principalmente para el desarrollo del kernel de Linux, pero hoy en día es utilizado por miles de proyectos en todo el mundo.

### ¿Por qué usar Git?

Git es útil porque permite a los equipos de desarrollo trabajar en conjunto en el mismo código, manteniendo un registro detallado de los cambios realizados y facilitando la integración de diferentes contribuciones sin conflictos. Algunas de sus características principales son:

- **Rápido y eficiente**: Git es conocido por su velocidad, especialmente en grandes proyectos.
- **Distribuido**: A diferencia de otros sistemas, Git permite que cada desarrollador tenga una copia completa del historial del proyecto.
- **Control de versiones**: Git guarda un historial completo de cambios, lo que facilita revertir errores o entender cómo se desarrolló el proyecto.
  
### Instalación de Git

Si aún no tienes Git instalado en tu sistema, puedes instalarlo siguiendo estos pasos:

- **Windows**: Descarga el instalador desde [git-scm.com](https://git-scm.com/download/win) y sigue el asistente de instalación.
- **macOS**: Usa Homebrew para instalar Git con el siguiente comando:

  ```bash
  brew install git

- **Linux**: Usa el gestor de paquetes de tu distribución. Por ejemplo, en Ubuntu o Debian


                    

### Introducción a Markdown

1 Introducción

Ya aprendimos algo de Markdown en la asignatura FuComp, como parte de los notebooks de Jupyter:

Esta celda es **Markdown**, y por tanto, es ignorada por el *kernel*

Existen infinitos ejemplos de texto markdown, por ejemplo, en github:

Por ejemplo, [este tutorial](https://github.com/luong-komorebi/Markdown-Tutorial) sobre **Markdown**, escrito en **Markdown** (readme.md)

[Este notebook](https://github.com/jakevdp/PythonDataScienceHandbook/tree/master/notebooks) es un ejemplo de la potencia de Markdown (y además un buen manual)

[Y este otro entorno (colab)](https://colab.research.google.com/) te permite utilizar los notebooks sin instalar nada.


[Finalmente, tenemos aquí un chuletario](http://github.com/downloads/ahrencode/Miscellaneous/markdown-cheatsheet.pdf)

2 Estilos

_Prueba esto en whatsapp_

~~Prueba esto en whatsapp~~

**Prueba esto en whatsapp**

```c++ 
main(); \\ Prueba esto en whatsapp
```
<br>


### VSCode

Visual Studio Code es el editor que se recomienda para trabajar con este repositorio, tanto para leer y editar archivos Markdown, Python, C/C++ y otros recursos de la asignatura.

#### Instalación

- Descárgalo desde la página oficial: [https://code.visualstudio.com/](https://code.visualstudio.com/)
- Instala la versión correspondiente a tu sistema operativo.
- Tras la instalación, abre VS Code y asegúrate de tenerlo actualizado.

#### Extensiones recomendadas

Para esta asignatura conviene instalar, al menos, estas extensiones:

- [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [GitHub Copilot](https://marketplace.visualstudio.com/items?itemName=GitHub.copilot)
- [GitHub Copilot Chat](https://marketplace.visualstudio.com/items?itemName=GitHub.copilot-chat)
- [Python](https://marketplace.visualstudio.com/items?itemName=ms-python.python) (si vas a trabajar con notebooks o scripts Python)
- [Markdown All in One](https://marketplace.visualstudio.com/items?itemName=yzhang.markdown-all-in-one) (muy útil para leer y editar documentación)

#### Uso básico

- Abre esta carpeta del repositorio en VS Code usando "File > Open Folder...".
- Usa la terminal integrada para ejecutar comandos de Git, compilación y pruebas.
- Si trabajas con C/C++, es recomendable compilar desde una terminal y usar la extensión de C/C++ para autocompletado y depuración.

#### Configuración recomendada

- Activa la fuente de tu preferencia y ajusta la terminal a tu shell.
- Configura Git para tu usuario si aún no lo has hecho:

  ```bash
  git config --global user.name "Tu Nombre"
  git config --global user.email "tu-email@uma.es"
  ```

---

### Copilot

GitHub Copilot es una herramienta de asistencia basada en IA que puede ayudarte a:

- entender mejor fragmentos de código,
- escribir comentarios y documentación,
- sugerir correcciones y mejoras,
- acelerar la resolución de ejercicios y prácticas.

#### Para estudiantes

Si eres estudiante, normalmente puedes activar Copilot con tu cuenta de GitHub educativa o con la versión estudiantil disponible a través de GitHub Education.

#### Cómo activar la cuenta de estudiante

1. Accede a GitHub Education: [https://education.github.com/](https://education.github.com/)
2. Busca la opción de "Student Developer Pack" o "GitHub Copilot for Students".
3. Solicita la verificación como estudiante usando tu correo universitario o documentación que te pida GitHub.
4. Cuando tu solicitud sea aprobada, entra en: [https://github.com/features/copilot](https://github.com/features/copilot)
5. Activa GitHub Copilot con tu cuenta de GitHub y confirma que tu plan está disponible.

#### Cómo usarlo en VS Code

1. Instala la extensión [GitHub Copilot](https://marketplace.visualstudio.com/items?itemName=GitHub.copilot) en VS Code.
2. Instala también [GitHub Copilot Chat](https://marketplace.visualstudio.com/items?itemName=GitHub.copilot-chat).
3. Inicia sesión con tu cuenta de GitHub desde la extensión.
4. Una vez activado, podrás usar sugerencias automáticas y chat de ayuda dentro del editor.

> Si tienes problemas con la activación, revisa primero la página oficial de GitHub Education y la documentación de Copilot: [https://github.com/features/copilot](https://github.com/features/copilot)

### Referencias

* [Sitio web del CV](https://eii.cv.uma.es/course/view.php?id=193)
* [Repositorio de Fundamentos de Computadores](https://github.com/luisfromero/fucomp/)
* [Casium](https://casium.uma.es)
* [Desarrollo remoto con vscode](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.vscode-remote-extensionpack)




