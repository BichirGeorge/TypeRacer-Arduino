# TypeRacer-Arduino
## Descriere
* Acest proiect simulează un joc de tip TypeRacer folosind un microcontroler folosind LED-uri și două butoane.
	* Jocul poate avea două stări: „repaus” (indicat prin aprinderea LED-ului RGB alb) și „rundă activă” (în care LED-ul RGB devine verde pentru răspunsuri corecte și roșu pentru greșeli)
	* Apăsarea butonului de start inițiază jocul printr-o numărătoare inversă, iar jucătorul trebuie să scrie corect cuvintele afișate într-un timp limitat.
	* Butonul de dificultate permite schimbarea nivelului de dificultate (Easy, Medium, Hard), ajustând timpul de reacție.
  * Pentru a prinde corect apăsarile butoanelor am folosit tehnica de debouncing, iar funcțiile sunt controlate prin întreruperi.
## Componente utilizate
* Arduino UNO (ATmega328P microcontroller)
* 1x LED RGB (pentru a semnaliza dacă cuvântul corect e scris greșit sau nu)
* 5x Rezistoare (3x 220, 2x 1K)
* 2x Butoane (pentru start/stop rundă și pentru selectarea dificultății)
* Fire de legătură
* Breadboard
## Setup fizic
![1](https://github.com/user-attachments/assets/7344dc9a-e499-4f68-be12-5d5acfb0b76c)
![2](https://github.com/user-attachments/assets/e74e1fed-6876-4d1c-894a-b6973035b680)
![3](https://github.com/user-attachments/assets/863f6be2-7c0c-4142-a424-ecd15abdb798)
## Funcționalitatea montajului fizic
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/M8gUaeYEaQs/0.jpg)](https://www.youtube.com/watch?v=M8gUaeYEaQs)

:point_down:       Butonul de dificultate    :point_down:

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/fwuriFjDXCg/0.jpg)](https://www.youtube.com/watch?v=fwuriFjDXCg)
## Schema Electrică
![image](https://github.com/user-attachments/assets/f3395b92-b543-4d00-a230-51e12ddc6801)
