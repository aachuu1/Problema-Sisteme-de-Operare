# Problema-Sisteme-de-Operare
**Cerinta**
  Scrieti un program care numara aparitiile unui numar intr-un vector
 (numarul si elementele vectorului sunt date ca argumente in linia de
 comanda) printr-o strategie de tip divide et impera: se imparte vectorul
 in doua, apoi se initiaza cate un thread, folosind biblioteca 'Pthreads',
 care cauta numarul in fiecare jumatate, in aceeasi maniera (fiecare
 thread va genera, daca e cazul, alte doua thread-uri); cele doua
 thread-uri se desfasoara in paralel. Fiecare thread va furniza ca
 informatie numarul de aparitii, adunand acest numar la o variabila
 globala protejata de un mutex. Procesul initial va afisa numarul total
 obtinut. Atat procesul cat si orice thread generat nu se vor termina
 decat dupa ce se vor termina toate thread-urile generate de el.
