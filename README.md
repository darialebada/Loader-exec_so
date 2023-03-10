###### Lebada Daria-Cristiana 323CA
## Loader de executabile

#### Explicatii ierarhie fisiere:
- in skel-lin se gaseste scheletul de cod
- in checker-lin se gaseste checkerul (pentru Linux)
#### Implementarea propriu-zisa:
- skel-lin/loader/loader.c

### Descrierea implementarii:
Se doreste implementarea unui loader de fisiere executabile in
format ELF pentru Linux sub forma unei biblioteci dinamice.
Implementez o functie de tip handler prin care voi trata page
fault-urile interceptate. Primul pas este identificarea segementului
si a paginii in care se gaseste adresa care provoaca page fault-ul. <br />
Exista 3 cazuri posibile pe care le tratez in implementarea mea:
pagina este deja mapata sau pagina nu se regaseste in niciun segment,
unde se va rula handler-ul default si cazul in care pagina gasita se
gaseste intr-un segment si nu a fost mapata. In acest ultim caz se
identifica adresa paginii in care exista page fault-ul si se mapeaza,
dandu-i permisiuni de scriere/ citire pentru a putea edita datele din
aceasta. Apoi se identifica cazul care trebuie tratat pentru copierea
datelor:
- Pagina se afla in zona memoriei din fisier -> se copiaza datele;
- Pagina se afla inafara zonei de memorie a fisierului -> se
zeroizeaza intreaga pagina;
- Pagina se afla partial in zona memoriei din fisier -> se copiaza
datele din zona aflata in fisier si se zeroizeaza partea ramasa.\
In final se modifica permisiunile adresei mapate cu cele ale fisierului.


