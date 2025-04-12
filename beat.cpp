#include <iostream>
#include <vector>
#include <cmath>
#include <sndfile.h>
#include <fftw3.h>
#include <ncurses.h>

#define FRAME_SIZE 1024

using namespace std;

void drawBars(const vector<double> & magnitudes){
    clear();
    int cols = COLS;
    int height = LINES;

    for(int i = 0; i<cols;++i){
        int index = i * magnitudes.size() / cols;
        int barHeight = min(static_cast<int>(magnitudes[index] * 40), height -1);

        for(int j=0; j<barHeight; ++j){
            mvprintw(height - j - 1,i,"|");
        }
    }

    refresh();
}

int main(){

    SF_INFO sfinfo;
    SNDFILE* file = sf_open("sample.wav",SFM_READ, &sfinfo);
    if(!file){
        cerr << "Erro ao abrir o arquivo de som \n";
        return 1;
    }

    initscr();
    noecho();
    curs_set(FALSE);
    
    vector<double> buffer(FRAME_SIZE);
    vector<double> magnitudes(FRAME_SIZE /2);

    fftw_complex* out= (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FRAME_SIZE);
    fftw_plan plan= fftw_plan_dft_c2r_1d(FRAME_SIZE,buffer.data(),out,FFTW_ESTIMATE);

    while (sf_read_double(file, buffer.data(),FRAME_SIZE)== FRAME_SIZE);
    {
        fftw_execute(plan);

        for(int i = 0;i<FRAME_SIZE / 2; ++i){
            magnitudes[i] = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
        }
        drawBars(magnitudes);
        napms(30);
    }

    fftw_destroy_plan(plan);
    fftw_free(out);
    sf_close(file);
    endwin();

    return 0;
}