# Strategie
* Mise sous tension -> initialisation servo moteur (pied en particulier pour avoir la position de depart)
* Appuie sur boutojn cote adversaire (bouton droit ou bouton gauche)
  - detection boutton doit renvoyer droit(1) ou gauche(3)
* Attendre 5 secondes
* Demarrage jupe
* Attaque Aveugle
  - avance(droit)
  - apeler la fonction detect -> avance (avec firection de detec)
  - avance(gauche)
  - apeler la fonction detect
  et on boucle pendant 3 pas de chaque cote sauf si detection (si on a implementer les interruptions) - on commence par droite ou gauche en fonction du bouton apuyer pour le demarrage

* Detection
  - si detection avance
  - si non Tourne 10°
  - Renvoie la DIRECTION aka le numero du capteur qui a detecter
* Avance
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
  
