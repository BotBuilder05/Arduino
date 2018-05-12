# Okatoho V3 - Pin list

* Capteurs IR
  * Avant   : A6
  * Droite  : A3
  * Gauche  : A2
  * Arriere : A1
* Valeurs deuils des capteur IR
  * Obstacle à  4  cm -> Valeur : 
  * Obstacle à  40 cm -> Valeur : 

* Moteur commande de la jupe
  * Marche HIGH (+5v)   : 5
  * Marche LOW (Gnd) : 6

* Leds
  * 0 => éteint   1 => allumé
  * Led oeil : 11
  * Led haut de chapeau : 4
  * Coté du corp 1 : 7 
  * Coté du corp 2 : 10

* Servo
  * Servo Mouvement : 9
  * Servo Direction : 3

* Boutons 
  * Bouton droite : 12
  * Bouton gauche oeil : 14 (A0)
  * Bouton sur la tete : On/Off non connecter a l’arduino (coupe-circuit)
  
* voltmetre
  * mesure 1/2 tention de la batterie


# Strategie
* Mise sous tension -> initialisation servo moteur (pied en particulier pour avoir la position de depart)
* Appuie sur boutojn cote adversaire (bouton droit ou bouton gauche)
  - detection boutton doit renvoyer droit(1) ou gauche(3)
* Attendre 5 secondes
* Demarrage jupe
* Attaque Aveugle
	- on commence par devant ou gauche en fonction du bouton apuyer pour le demarrage
  - avance(devant) ou gauche
  - apeler la fonction detect -> si Vrai : passe a l'ETAT Attaque (avec direction de detec)
  - avance(devant) ou arriere
  - apeler la fonction detect -> si vrai : idem
  et on boucle pendant 3 pas de chaque cote sauf si detection (si on a implementer les interruptions) 

* Detection
  - si detection avance
  - si non Tourne 10° - on detecte robot en bas
  - Renvoie la DIRECTION aka le numero du capteur qui a detecter
  
* Attaque
  - Voir programme etat marche 
  - Avec DIRECTION on lit la matrice TabDirPied
Si pied en position avant -> on lit la ligne 0
Si pied en position arriere -> on lit la ligne 1 
On lit la colone qui correcton a la DIRECTION
  - direction arriere : 0
  - direction droite  : 1
  - direction avant   : 2
  - direction gauche  : 3

  - Ensuite on effectue le pas
 

Etat Detection
Fonction detection
 - lit 1 fois chaque capteurs et on boucle 2 fois -> TABLEAUX
 - Detection si les 2 valeurs pour un meme capteurs sont superieures au seuil
 - Renvoir si detection ou non (boolean)
 - Affecte la variable de direction si detection

 
Fonction tourne de x°
  - on met le servo de marche (servoM) dans la position basse
  - on met le servo de direction (servoD) line 84 de etat marche +10 
  - on met le servo de marche (servoM) dans la posistion en debut de fonction tourne 10
  - on remet le pied a l'origine on met le servo de direction (servoD) line 84 de etat marche:w


