## 📋 Plan de creación de un Engine Terminal para Juegos (ncurses/FTXUI Ready)

### 1. **Identificar necesidades comunes de los juegos actuales**
- Gestión de mapas/tableros (carga desde archivos, representación en memoria, renderizado)
- Dibujo/renderizado de sprites de texto/símbolos/ASCII en distintas posiciones, con color
- Manejo y procesamiento de entrada del usuario (teclas, combinaciones)
- Gestión de entidades: jugador, enemigos, objetos
- Loop principal de juego y actualización por frame/turno
- Manejo de estados del juego (inicio, running, pausa, fin)
- Implementación de UI: mensajes, HUD, menús, score
- Control de flujo/escenas (pasar de menú, a juego, a game over, etc.)
- Control de concurrencia/multihilo (para enemigos o animaciones)
- Incorporación de audio: música, efectos de sonido
- Soporte multiplataforma: al menos Linux/macOS, fácil de portar a Windows

---

### 2. **Diseño modular del engine**

**Módulos/Sistemas principales:**
1. **Mapas:**
   - Parser de mapas desde archivos texto/JSON
   - Renderizador de tablero/celdas (paredes, espacios, objetos)
   - Soporte para layers/capas futuras (background, entidades)
   
2. **Render/Formas gráficas:**
   - API de dibujo simple orientada a sprites de texto, color, estilos
   - Abstracción para “Sprite” o “Entidad dibujable”
   - Capacidad de doble buffer/redibujado para evitar flicker

3. **Input:**
   - Gestor de teclas (incluyendo combinaciones y repetición)
   - Callbacks/Binding por tecla

4. **Entidades y lógica:**
   - API para crear entidades (posición, tipo, métodos de update y render)
   - Motor de colisiones si es necesario
   - Control de ciclos de vida (spawn, destroy)

5. **Loop principal:**
   - Gestión de tiempos, FPS/turnos, pausas, eventos globales

6. **UI:**
   - Soporte a mensajes, textos, menús, selección
   - HUD: puntuación, vidas, información contextual
   - Ventanas emergentes/informativas

7. **Sonido:**
   - Soporte a audio terminal (p. ej., usando SoX, aplay, o MP3/WAV simple vía librerías como SDL_mixer, sox, o sistemas de beep)
   - API para música de fondo y efectos de sonido (trigger por evento)

8. **Plataforma:**
   - Inicialización, escalado de UI, gestión de ventanas

9. **Futuro/Compatibilidad FTXUI:**
   - Separar núcleo de lógica y rendering, de modo que el render sea un “backend” intercambiable (ncurses/future: FTXUI)
   - Evitar acoplamiento con ncurses en lógica central, limitarlo al backend de dibujo/UI

---

### 3. **Estrategia de implementación**

1. **Fase 1: Foundation (ncurses puro)**
   - Escribir núcleo modular básico (`libengine.a` o engine/).
   - Proveer una API clara estilo “engine_init, engine_loop, engine_draw_sprite, ...”.
   - Migrar cada juego al engine desde su lógica original de rendering y input.

2. **Fase 2: Audio**
   - Investigar solución viable en terminal:  
     - Opción simple: comando externo (aplay, afplay, paplay)
     - Mejor opción multiplataforma: integrar una lib pequeña como SDL2_mixer (solo para audio), o alguna minimalista en C++
   - Envolver API: `engine_play_sound`, `engine_play_music`

3. **Fase 3: Abstracción de backend visual**
   - Encapsular todas llamadas a ncurses en un renderer propio dentro del engine.
   - Definir interfaces para cambiar ncurses por FTXUI en el futuro sin romper la lógica central.

4. **Fase 4: Migración/upgrade a FTXUI (futuro)**
   - Implementar el nuevo backend de rendering (FTXUI), probando compatibilidad y mejoras visuales.
   - Permitir compilación con uno u otro backend mediante flag/cmake.

---

### 4. **Ventajas del enfoque**
- Los juegos existentes se beneficiarán de mejoras futuras de manera centralizada.
- Simplifica el mantenimiento (una sola fuente de rendering, input, audio).
- Facilita la incorporación de nuevas features (p. ej., animaciones, nuevos efectos).
- Permite fácil paso a FTXUI (mejor rendering, widgets, mouse).

---

### 5. **Ejemplo de API mínima de uso (ficticia)**

```cpp
#include <engine/engine.h>
using namespace engine;

int main() {
    EngineConfig cfg{/*...*/};
    engine::init(cfg);

    Map map = engine::load_map("level1.txt");
    engine::set_map(map);
    engine::add_entity(Player{});
    engine::add_entity(Ghost{});

    engine::play_music("main_theme.wav");

    engine::run([]() {
        // update lógicas
    });
    return 0;
}
```

---

### 6. **Verificación de Juegos Existentes con Bazel**

Para asegurar que todos los juegos siguen funcionando correctamente tras cambios en el engine, se recomienda compilar y
ejecutar los binarios principales de cada uno. Los comandos de Bazel típicos son:

```sh
# Compilar todos los binarios (incluyendo todos los juegos)
bazel build //...

# Ejecutar un juego específico
bazel run //galaga:galaga
bazel run //hunter:hunter
bazel run //pacman:pacman
bazel run //sokoban:sokoban
bazel run //vimnet:vimnet
```

Opcionalmente, si existen tests automatizados definidos, pueden ejecutarse con:

```sh
bazel test //...
```

Es recomendable agregar estos comandos a la documentación o scripts de CI/CD del proyecto para asegurar compatibilidad
continua tras cualquier refactor o mejora del engine.

