# HTTP-proxy


## **Fluxul de Functionare**

1. **Pornirea serverului**
   - Dupa apasarea butonului **Start Server**, serverul se initializeaza si incarca lista de domenii blocate din fisierul `blocked_domains.txt`.

2. **Acceptarea conexiunilor**
   - Proxy-ul accepta noi conexiuni de la browser si creeaza un socket dedicat pentru fiecare client.

3. **Primirea cererilor**
   - Browser-ul trimite cereri HTTP sau HTTPS catre proxy.
   - Proxy-ul parseaza cererile pentru a identifica metoda HTTP (ex. GET, POST) si URL-ul cerut.

4. **Verificarea blacklist-ului**
   - Domeniul cerut este comparat cu lista din `blocked_domains.txt`.
   - **Daca domeniul este blocat**, proxy-ul raspunde cu:
     ```http
     HTTP/1.1 403 Forbidden
     ```
   - **Daca domeniul nu este blocat**, cererea este procesata mai departe.

5. **Transmiterea cererilor**
   - Cererile sunt trimise catre serverul tinta:
     - Prin **QTcpSocket** pentru HTTP.
     - Prin **QSslSocket** pentru HTTPS.

6. **Transmiterea raspunsurilor**
   - Proxy-ul primeste raspunsul de la serverul tinta si il transmite browser-ului client.

7. **Logging**
   - Detaliile fiecarei cereri si ale raspunsului sunt salvate in fisierul `firefox.txt` pe Desktop.

8. **Conexiuni multiple**
   - Proxy-ul gestioneaza mai multe conexiuni simultane folosind un **ThreadPool**, ceea ce optimizeaza utilizarea resurselor.

---

## **Exemple Demo**

### **1. Cerere permisa**
**URL:** `http://httpforever.com`

- Proxy-ul verifica domeniul in blacklist si constata ca nu este blocat.
- Cererea este transmisa catre serverul `httpforever.com`.
- Browser-ul primeste raspunsul.

### **2. Cerere blocata**
**URL:** `http://youtube.com`

- Proxy-ul identifica domeniul `youtube.com` in lista de domenii blocate.
- Raspunsul trimis catre browser este:
  ```http
  HTTP/1.1 403 Forbidden

  ---

## **Functionalitati care nu functioneaza complet**

- **Raspunsuri mari**: Daca un raspuns depaseste o anumita dimensiune, aplicatia poate crapa.
- **Logging incomplet**: Raspunsurile partiale nu sunt tot timpul salvate corect in fisierul de log.
- **Headere fixe**: In acest moment, nu putem adauga, modifica sau sterge headerele trimise in cererile HTTP/HTTPS.


