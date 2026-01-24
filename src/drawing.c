#include "../include/drawing.h"
void draw_particles(Particle* p, int n)
{
	for(int i = 0; i<n; ++i)
	{
		DrawCircle(p[i].pos.x, p[i].pos.y,p[i].r, p[i].color);
	}
	return;
}


int draw_diagnostics(double frametime_start, double frametime_end, double render_start, double render_end, double update_start, double update_end)
{


		double tft = 1000 * (frametime_end - frametime_start);
		double trt = 1000 * (render_end - render_start);
		double tut = 1000 * (update_end - update_start);
		double over = tft - trt - tut;
		const char* frametime = "total frametime: %.3f ms";
		int ft_len = strlen(frametime) + 10;
		char ftbuf[ft_len];
		snprintf(ftbuf,ft_len,CLITERAL(frametime),tft);

		const char* render = "render time: %0.3f ms";
		int re_len = strlen(render) + 10;
		char rebuf[re_len];
		snprintf(rebuf,re_len,CLITERAL(render), trt);

		const char* update = "update time: %0.3f ms";
		int up_len = strlen(update) + 10;
		char upbuf[up_len];
		snprintf(upbuf,up_len,CLITERAL(update), tut);

		const char* overhead = "overhead time: %0.3f ms";
		int ov_len = strlen(overhead) + 10;
		char ov_buf[ov_len];
		snprintf(ov_buf,ov_len,CLITERAL(overhead), over );

		DrawText(ftbuf,10,30,22,WHITE);
		DrawText(rebuf,10,60,22,WHITE);
		DrawText(upbuf,10,90,22,WHITE);
		DrawText(ov_buf,10,120,22,WHITE);


		return 0;
}



void draw_gui(Options* props)
{
	GuiSliderBar((Rectangle){100,175,100,25},"Particle Count",TextFormat("%d",(int)props->nparticles),&props->nparticles,1,100);

	GuiSliderBar((Rectangle){100,225,100,25},"Gravity",TextFormat("%0.2f",props->gravity),&props->gravity,0,50);

	// GuiSliderBar((Rectangle){100,275,100,25},"Timestep",TextFormat("1/%d",props->timestep),&props->timestep,1/1.,1/200.);

}













