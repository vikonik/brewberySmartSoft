#ifndef _MENU_RECEPT_LOADED_H
#define _MENU_RECEPT_LOADED_H

void startRecepteProcessLoaded(void);
	
void receptSetFunctionLoaded(void);
/*
Выводим на экран таблицу для отображеия хода автоматического процесса
чтобы не плодить буфера используем receptControl[0]
*/
//void printShablonRecepteLoaded(Recipe_t *recept, SystemState_t* processState);


/*
Запускаем автоматический процесс
*/
void shablonSetFunctionAutoLoaded(void);

/**/
void shablonSetFunctionAutoResnartLoaded(void);
/**/
void shablonSetFunctionFilnrationAutoResnartLoaded(void);

/**/
void shablonSetFunctionBoilingAutoResnartLoaded(void);


/**/
void shablonSetFunctionCoolingAutoResnartLoaded(void);

/**/
void shablonSetFunctionFermentingAutoResnartLoaded(void);

/**/
void stopProcessLoaded(void);

#endif 
