# Miniseră automată RTOS
Tema proiectului este o miniseră automată. Aceasta are ca scop monitorizarea și întreținerea automată a plantelor, asigurând temperatura optimă și gradul de irigație potrivit.
# Arhitectura sistemului și logica proiectului
![transmisia_bloc](https://user-images.githubusercontent.com/54595551/98228377-2f40d980-1f61-11eb-87e6-ef77fc947e2b.png)
![receptia_bloc](https://user-images.githubusercontent.com/54595551/98228436-4384d680-1f61-11eb-94e2-5ce4da04723a.png)

Proiectul constă din două părți: una de transmisie și una de recepție.

Partea de transmisie reprezintă colectarea datelor de la senzori și transmiterea lor wireless. Temperatura și umiditatea din aer sunt măsurate cu ajutorul unui senzor (DHT11), ce are ieșire digitală. Datele măsurate sunt transmise la placa Arduino printr-un pin digital.Umiditatea solului este măsurată cu ajutorul unui alt senzor (YL-69), ce are ieșire analogică. Datele în urma măsurării sunt transmise la placă prin intermediul unui pin analogic.
Toate aceste date vor fi transmise prin intermediul modulului wireless NRF4L01.

Partea de recepție reprezintă controlul mini pompei de apă și afișarea datelor colectate de către senzori pe LCD (temperatura în grade, umiditatea în procent). Datele transmise sunt recepționate cu ajutorul modulului NRF24L01.
În ceea ce privește mini poma de apă, aceasta este comandată cu ajutorul unui releu. Aceasta pornește când umiditatea măsurată de senzorul de umiditate a solului este mai mare sau egală cu 20% (cu cât este mai mare, cu atât solul este mai uscat). După ce pornește și udă un anumit timp, se vor mai face verificări ale umidității după o oră deoarece trebuie să se stabilizeze umiditatea solului în urma acționării pompei cu apă.

![schema_bloc_soft](https://user-images.githubusercontent.com/54595551/103291123-be172300-49f3-11eb-9c8e-ed036c4f215c.png)
 
