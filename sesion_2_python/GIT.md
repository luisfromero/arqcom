### Introducción a GIT

El control de versiones es una de las tareas fundamentales para la administración de un proyecto de desarrollo de software en general, pero también en otros ámbitos, como puede ser la gestión de una asignatura o incluso la recopilación de recetas de cocina. Surge de la necesidad de mantener y llevar control del código o los textos que vamos creando, conservando sus distintos estados intermedios. Es absolutamente necesario para el trabajo en equipo, pero resulta útil incluso a desarrolladores independientes.

Aunque trabajemos solos, sabemos más o menos cómo surge la necesidad de gestionar los cambio entre distintas versiones de un mismo código. Prueba de ello es que todos los programadores, más tarde o más temprano, se han visto en la necesidad de tener dos o más copias de un mismo archivo, para no perder su estado anterior cuando vamos a introducir diversos cambios. Para ir solucionando nuestro día a día habremos copiado un fichero, agregándole la fecha o un sufijo como "antiguo". Aunque quizás esta acción nos sirva para salir del paso, no es lo más cómodo ni mucho menos lo más práctico.

Git es un sistema de control de versiones distribuido que se diferencia del resto en el modo en que modela sus datos. La mayoría de los demás sistemas almacenan la información como una lista de cambios en los archivos, mientras que Git modela sus datos más como un conjunto de instantáneas de un mini sistema de archivos.

#### Las ramas (branches) de GIT

Las ramas son una división del estado del código, esto permite crear nuevos caminos a favor de la evolución del código. Por ejemplo, imagina que quieres añadir una nueva función o tal vez arreglar un error, sin importar su tamaño, generas una nueva rama en la cual se alojan estos cambios que realizaste, al realizar esta acción va resultar más complicado que algún error o fallo del código inestable se incorpore al código base principal, dando la oportunidad de limpiar tu historial antes de fusionarlo todo con la rama principal, mejorando tu eficiencia de trabajo. Las ramas se pueden fusionar (merge), y de hecho, es lo habitual en los proyectos.

#### Los tres estados de GIT

Git tiene tres estados principales en los que se pueden encontrar tus archivos: confirmado (committed), modificado (modified), y preparado (staged). Confirmado significa que los datos están almacenados de manera segura en tu base de datos local. Modificado significa que has modificado el archivo pero todavía no lo has confirmado a tu base de datos. Preparado significa que has marcado un archivo modificado en su versión actual para que vaya en tu próxima confirmación.

Esto nos lleva a las tres secciones principales de un proyecto de Git: el directorio de Git (Git directory), el directorio de trabajo (working directory), y el área de preparación (staging area).

Git (localmente) tiene un directorio (.git) en el que envías tus archivos, que se conoce como 'repositorio local'. 

#### Github

No es más que un almacén remoto de repositorios, de los muchos que hay. Es posiblemente el más conocido, pero no necesariamente el más adecuado para ciertos proyectos académicos.

#### Operaciones con los repositorios

* git init  (iniciar repositorio)
* git add (agregar archivos)
* git add remote (asocia un repositorio local con otro remoto)
* git branch (trabajar con ramas: crear, modificar, borrar)
* git fetch (consulta ramas remotas de un repositorio, sin descargar archivos. Solo metadatos)
* git pull (descarga los archivos)


### Cómo empezar a trabajar con GIT?


Recomiendo encarecidamente que, para iniciarse con Git, se modifiquen exclusivamente las copias locales (en tu ordenador) del repositorio. Para ello:

* Instala git para Windows/Linux, ...
* Crea, o elige un directorio del proyecto de tu ordenador
* Crea un archivo README.md (ejecutando la primera línea del código siguiente, en una ventana de comandos, en el directorio del proyecto)
* Crea el repositorio local (.git) en tu directorio del proyecto, usando eel comando **git init**
* Indica que todos los archivos de esa carpeta forman parte del repositorio, con **git add .**
* Opcionalmente, pero recomendado (si deseas usar Github), crea un repositorio remoto **vacío, sin readme**, en tu cuenta de github, con un nombre, p.e. ac1
* Crear el primer commit de tu repositorio, y una rama para dicho commit
* Vincula el repositorio de github con el repositorio local: *git remote add origin https://github.com/luisfromero/ac1.git**
* Envía la rama main al repositorio remoto


```
echo "# Proyecto AC1" > README.md
git init
git add .
git commit -m "commit inicial"
git branch -M main
git remote add origin https://github.com/luisfromero/ac1.git
git push -u origin main
```

A partir de ahora, ya es conveniente utilizar algún entorno que tenga Git integrado para trabajar con tu proyecto (CLion,VSCode,AndroidStudio,VisualStudio, etc.)

Por ejemplo, VSCode, los pasos serían...

* Editar uno o varios archivos (por ejemplo, README.md). Guardarlo con ctrl-S (El archivo está en estado 1: modificado)
* En la pestaña izuierda, de source control, README aparece en la lista de changes. Pulsa el + justo al nombre del archivo README
* Ahora el archivo está en el estado 2 (preparado, staged)
* Haz lo mismo con otros archivos, si quieres.
* Cuando creas que quieres consolidar (commit), ponle un texto al mensaje de consolidación, ej. "Ampliación del manualillo de git", y pulsa commit.

Opcionalmente, sincroniza el repositorio remoto, tras cada commit. O cada dos o tres commits, si no hay nadie esperando tus cambios.