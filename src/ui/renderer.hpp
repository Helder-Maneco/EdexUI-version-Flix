#ifndef RENDERER_HPP
#define RENDERER_HPP 

#include <termbox.h>
#include <string>
#include <tuple>
#include <vector>
#include "../core/stats_collector.hpp"

enum class ColorTheme 
{
    Default,
    Dark,
    Light,
    Monochrome
};

class UIRenderer
{
    private:
	bool initialized = false;
	ColorTheme theme = ColorTheme::Dark;

	//Definições de cor baseadas no tema
	std::tuple<uint8_t, uint8_t> accentColor;
	std::tuple<uint8_t, uint8_t> bgColor;
	std::tuple<uint8_t, uint8_t> textColor;

	//Config Layout
	int width;
	int height;
	int refreshRateMs;

    public:
	UIRenderer(int w = 80, int h = 24);
	~UIRenderer();

	//Inicializa Termbox
	booll init();

	//Renderiza UI completa
	void render(const CPUStats& cpu, const MemStats& mem, 
		    const NetStats& net = {}, const DiskStats& disk = {});

	//Renderiza apenas grafico CPU (para otimizacao)
	void renderCPUGraph(double percentage);

	//Renderiza painel de texto simples
	void renderTextPanel(const std::string& title, const std::string& content);

	//Limpa screen antes da renderizacao
	void clearScreen();

	//Atualiza tema Dinamico
	void setTheme(ColorTheme newTheme);

	//fecha termbox
	void shutdown();

    private:
	void drawBox (int x, int y, int w, int h, const std::string& title);
	void drawBar (int x, int y, int w, int h, float fillPercent, float color);
	void drawASCIIChart (std::vector<int>& data, int x, int y, int w, int h);
	void setColor (uint8_t fg, uint8_t bg);

    public:
	//Getters para configuracoes
	int getWidth() const {return  width;}
	int getHeight() const {return height;}
};
#endif //RENDERER_HPP
