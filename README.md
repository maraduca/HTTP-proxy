# HTTP-proxy

# Proxy HTTP/HTTPS cu Blacklist

## **Fluxul de Funcționare**

1. **Pornirea serverului**
   - După apăsarea butonului **Start Server**, serverul se inițializează și încarcă lista de domenii blocate din fișierul `blocked_domains.txt`.

2. **Acceptarea conexiunilor**
   - Proxy-ul acceptă noi conexiuni de la browser și creează un socket dedicat pentru fiecare client.

3. **Primirea cererilor**
   - Browser-ul trimite cereri HTTP sau HTTPS către proxy.
   - Proxy-ul parsează cererile pentru a identifica metoda HTTP (ex. GET, POST) și URL-ul cerut.

4. **Verificarea blacklist-ului**
   - Domeniul cerut este comparat cu lista din `blocked_domains.txt`.
   - **Dacă domeniul este blocat**, proxy-ul răspunde cu:
     ```http
     HTTP/1.1 403 Forbidden
     ```
   - **Dacă domeniul nu este blocat**, cererea este procesată mai departe.

5. **Transmiterea cererilor**
   - Cererile sunt trimise către serverul țintă:
     - Prin **QTcpSocket** pentru HTTP.
     - Prin **QSslSocket** pentru HTTPS.

6. **Transmiterea răspunsurilor**
   - Proxy-ul primește răspunsul de la serverul țintă și îl transmite browser-ului client.

7. **Logging**
   - Detaliile fiecărei cereri și ale răspunsului sunt salvate în fișierul `firefox.txt` pe Desktop.

8. **Conexiuni multiple**
   - Proxy-ul gestionează mai multe conexiuni simultane folosind un **ThreadPool**, ceea ce optimizează utilizarea resurselor.

---

## **Exemple Demo**

### **1. Cerere permisă**
**URL:** `http://httpforever.com`

- Proxy-ul verifică domeniul în blacklist și constată că nu este blocat.
- Cererea este transmisă către serverul `example.com`.
- Browser-ul primește răspunsul.

### **2. Cerere blocată**
**URL:** `http://ads.example.com`

- Proxy-ul identifică domeniul `youtube.com` în lista de domenii blocate.
- Răspunsul trimis către browser este:
  ```http
  HTTP/1.1 403 Forbidden

