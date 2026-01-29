#include <tamtypes.h>
#include <kernel.h>
#include <gs_privileged.h>
#include <graph.h>
#include <draw.h>
#include <malloc.h>

// Definimos colores
#define COLOR_SKY_R 135
#define COLOR_SKY_G 206
#define COLOR_SKY_B 235

// Estructura que define bloques
struct blocks {
  // Añadir bloques aqui
}

// --- CLASE DEL MOTOR (C++) ---
class TerrariaEngine {
private:
    framebuffer_t fb;
    zbuffer_t zb;
    qword_t *packet;
    int screenWidth;
    int screenHeight;

public:
    // Constructor: Configura valores iniciales
    TerrariaEngine(int width, int height) {
        screenWidth = width;
        screenHeight = height;
        packet = (qword_t*)memalign(128, 2000); // Asignar memoria alineada
    }

    // Método para inicializar el hardware de PS2
    void initVideo() {
        graph_vram_clear();
        
        // Configurar modo NTSC Entrelazado
        graph_set_mode(GRAPH_MODE_INTERLACED, GRAPH_MODE_NTSC, GRAPH_MODE_FIELD, GRAPH_ENABLE);
        graph_set_screen(0, 0, screenWidth, screenHeight);
        graph_set_bgcolor(0, 0, 0);

        fb.width = screenWidth;
        fb.height = screenHeight;
        fb.psm = GS_PSM_32;
        fb.address = graph_vram_allocate(fb.width, fb.height, fb.psm, GRAPH_ALIGN_PAGE);

        zb.enable = 1;
        zb.method = ZTEST_METHOD_GREATER_EQUAL;
        zb.zsm = GS_ZLZ_32;
        zb.address = graph_vram_allocate(fb.width, fb.height, zb.zsm, GRAPH_ALIGN_PAGE);

        graph_initialize(fb.address, fb.width, fb.height, fb.psm, 0, 0);
    }

    // Método para dibujar un frame (fotograma)
    void render() {
        // Preparar el entorno de dibujo
        qword_t *q = draw_setup_environment(packet, 0, (framebuffer_t*)&fb, (zbuffer_t*)&zb);
        
        // Limpiar pantalla con color azul cielo
        q = draw_clear(q, 0, 2048 - 320, screenWidth, screenHeight, COLOR_SKY_R, COLOR_SKY_G, COLOR_SKY_B);
        
        // Enviar instrucciones al chip gráfico (GIF)
        dma_channel_send_normal(DMA_CHANNEL_GIF, packet, q - packet, 0, 0);
        dma_wait_fast();
    }

    // Método de sincronización
    void waitVsync() {
        graph_wait_vsync();
    }
};

// --- FUNCIÓN PRINCIPAL ---
int main(int argc, char *argv[]) {
    // Instanciamos nuestro objeto "Juego"
    TerrariaEngine juego(640, 448);

    // Inicializamos
    juego.initVideo();

    // Bucle infinito
    while(1) {
        juego.render();
        juego.waitVsync();
    }

    return 0;
}
