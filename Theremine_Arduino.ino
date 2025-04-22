// Importation des librairies
  #include <Volume3.h>
  #include <rgb_lcd.h>
  #include <Wire.h>
  #include <NewPing.h> 


// Définition des ports
  #define PHOTOCAPTEUR A0
  #define POTENTIOMETRE A1
  #define BOUTON 2
  #define ULTRASON 7
  #define HP 9


// Définition des constantes
  #define distance_max 70
  #define modemax 7

  enum Notes {
    silence = 0,
    do2, doD2, re2, reD2, mi2, fa2, faD2, sol2, solD2, la2, laD2, si2,
    do3, doD3, re3, reD3, mi3, fa3, faD3, sol3, solD3, la3, laD3, si3,
    do4, doD4, re4, reD4, mi4, fa4, faD4, sol4, solD4, la4, laD4, si4,
    do5, doD5, re5, reD5, mi5, fa5, faD5, sol5, solD5, la5, laD5, si5,
    do6, doD6, re6, reD6, mi6, fa6, faD6, sol6, solD6, la6, laD6, si6, 
    FIN
  };

  int reB2=doD2 ; int reB3=doD3 ; int reB4=doD4 ; int reB5=doD5 ; int reB6=doD6 ;
  int miB2=reD2 ; int miB3=reD3 ; int miB4=reD4 ; int miB5=reD5 ; int miB6=reD6 ;
  int solB2=faD2; int solB3=faD3; int solB4=faD4; int solB5=faD5; int solB6=faD6;
  int laB2=solD2; int laB3=solD3; int laB4=solD4; int laB5=solD5; int laB6=solD6;
  int siB2=laD2 ; int siB3=laD3 ; int siB4=laD4 ; int siB5=laD5 ; int siB6=laD6 ;


// Initialisation ds librairies
  rgb_lcd lcd;
  NewPing sonar(ULTRASON,ULTRASON,distance_max);


// Définition de variables globales
  bool appuie = false;
  int mode = 0;

  // Nom des musiques
  const char* nom_musique[] = {
    "HARRY POTTER", "    TETRIS", "   MYSTERE", "    VADOR", "  THE FORCE", "    MARIO"
  };

  // Mode discret ou continu
  const char* D_OR_C[]={
    "D", "C"
  };

  // nom des notes (Octaves 2 a 6)
  const char* nom_note[] = {
    "Silence",
    "Do2", "Do#2", "Re2", "Re#2", "Mi2", "Fa2", "Fa#2", "Sol2", "Sol#2", "La2", "La#2", "Si2",
    "Do3", "Do#3", "Re3", "Re#3", "Mi3", "Fa3", "Fa#3", "Sol3", "Sol#3", "La3", "La#3", "Si3",
    "Do4", "Do#4", "Re4", "Re#4", "Mi4", "Fa4", "Fa#4", "Sol4", "Sol#4", "La4", "La#4", "Si4",
    "Do5", "Do#5", "Re5", "Re#5", "Mi5", "Fa5", "Fa#5", "Sol5", "Sol#5", "La5", "La#5", "Si5",
    "Do6", "Do#6", "Re6", "Re#6", "Mi6", "Fa6", "Fa#6", "Sol6", "Sol#6", "La6", "La#6", "Si6"
  };

  // Fréquences des notes (Octaves 2 a 6)
  const float frequence[] = {
    0,
    65.4 , 69.3 , 73.4 , 77.8 , 82.4 , 87.3 , 92.5 , 98   , 103.8, 110  , 116.5, 123.5,
    130.8, 138.6, 146.8, 155.6, 164.8, 174.6, 185  , 196  , 207.6, 220  , 233  , 246.9,
    262  , 277  , 294.6, 311.2, 330  , 349  , 370  , 392  , 415  , 440  , 466  , 494  ,
    523  , 554  , 589  , 622  , 660  , 698  , 740  , 784  , 831  , 880  , 932  , 968  ,
    1046 , 1109 , 1175 , 1244 , 1318 , 1397 , 1480 , 1568 , 1661 , 1760 , 1865 , 1976
  };


void setup() {
  // Définition des Entrées/Sorties
  pinMode(HP, OUTPUT);
  pinMode(BOUTON, INPUT); 

  Serial.begin(9600);
  // Paramétrage de l'écran
  lcd.begin(16, 2);

  // Affichage du lancement
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("STARTING...");
  lcd.setCursor(3, 1);
  lcd.print("THEREM'INP");

  // Musique de lancement
  choix_musique(0);

  // Affichage de la consigne
  attente_ecran(12,0);
}


void loop() {
  // Lecture du potentiomètre
  int nb_potentio = analogRead(POTENTIOMETRE);
  // Assignation des gammes accessibles
  int nb_note=map(nb_potentio,0,1022,12,24);
  int nb_frequence=map(nb_potentio,0,1023,247,494);

  // Gestion du changement de mode
  if (digitalRead(BOUTON) == HIGH && appuie==false) { 
    appuie = true ; 
    if (mode < modemax){
      mode = mode+1;
    } else {
      mode=0;
    }
  } 
  if (digitalRead(BOUTON) == LOW) { 
    appuie = false;
  }

  // Lecture du volume via le photocapteur
  int volume = get_volume();
  // Lecture de la distance via le capteur ultrason
  float distance = sonar.ping_cm();
  
  // Mode instrument
  if (mode<2){
    // Si la distance < distance_max, on joue la note associée
    if (distance > 0) {
      float freq;
      // On sélectionne la note jouée
      int noteIndex = map(distance,0,distance_max,13,13+nb_note);
      char* nom = nom_note[noteIndex];

      // Mode discret
      if (mode == 0) {
        freq = frequence[noteIndex];
        
      // Mode Continu
      } else {
        freq = map(distance,0,distance_max,130, nb_frequence);
      }

      // On joue et on affiche la note
      vol.tone(HP, freq, volume);
      afficher_ecran(nom, nb_note, mode);

    } else {
      // On ne joue pas de note
      attente_ecran(nb_note, mode);
      vol.noTone();
    }
  }

  // Mode Boite à musique
  if (mode >= 2) {
    gestion_musique(mode);
  }

  delay(50); 
}


int get_volume(){
  // Lis la valeur du photocapteur et la transforme en volume
  int mesure = analogRead(PHOTOCAPTEUR);
  int volume=map(mesure,300,800,1023, 0);
  return volume;
}


void afficher_ecran(char* nom, int nb_note, int mode) {
  
  // Affiche la note jouée 
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Vous jouez un:");

  lcd.setCursor(0, 1);
  lcd.print(nom);

  // Affiche le nombre de note jouable
  lcd.setCursor(14, 1);
  lcd.print(nb_note);

  // Affiche le mode
  lcd.setCursor(15,0 );
  lcd.print(D_OR_C[mode]);
  
}


void attente_ecran(int nb_note, int mode){
  
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Jouez une");

  lcd.setCursor(6, 1);
  lcd.print("Note");

  // Affiche le nombre de note jouable
  lcd.setCursor(14, 1);
  lcd.print(nb_note);

  // Affiche le mode
  lcd.setCursor(15,0 );
  lcd.print(D_OR_C[mode]);
}


void gestion_musique(int indice){
  // Tant que le bouton est pressé, on ne fait rien
  while (digitalRead(BOUTON)==HIGH);{}

  // Lorsque le bouton est relaché
  if (digitalRead(BOUTON) == LOW) { 
    appuie = false;
  }
  // Affichage du nom de la musique
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print(nom_musique[indice-2]);

  // On sélectionne la musique correspondante
  choix_musique(indice);
}


void play(int melodie[][2], int indice) {
  // Initialisation de l'indice de la note dans la musique
  int i=0;

  // Tant que la partition n'est pas finie, et que le bouton n'est pas pressé
  while (melodie[i][0]!= FIN && indice==mode && digitalRead(BOUTON) == LOW) {
    // On sélectionne la fréquence, la durée et le nom de la note jouée
    int duree = melodie[i][1] ;
    float freq = frequence[melodie[i][0]] ;
    char* nom = nom_note[melodie[i][0]] ;

    // On incrémente l'indice de 1
    i+=1;

    if (freq==0) {
      // On joue un silence
      vol.noTone() ;

    } else { 
      // On lit le volume et on joue la note
      if (indice==0){
        int volume=1023;
        vol.tone(HP, freq, volume) ;

      } else{
        int volume = get_volume();
        vol.tone(HP, freq, volume) ;
      }
      
    } 

    if (indice!=0){
      // On affiche la note jouée
      lcd.setCursor(0, 1);
      lcd.print("       ");

      lcd.setCursor(0, 1);
      lcd.print(nom);
    }
    
    // On joue la durée de la note
    delay(duree);
  }

  // Si on vient d'appuyer sur le bouton
  if (appuie == false && digitalRead(BOUTON) == HIGH ) {
    // On ne joue plus de note
    vol.noTone();
    // Redéfinition de appuie pour garder en mémoire qu'appuie a déja été pressé
    appuie = true ; 

    // On change de mode
    if (mode < modemax){
      mode = mode+1;
    } else {
      mode=0;
    }
  }  
}


void choix_musique(int indice) {

  // boot
  if (indice == 0) {
    // Définition des constantes des durées de notes
    int n= 60000/180; int c= n/2; int b= n*2; int bp= n*3; int arp = 100;

    // Partition de la musique de lancement ("Supercell")
    int boot[][2] = {
      {reB4 ,arp}, {solB4 ,arp}, {laB4 ,arp}, {reB5 ,arp}, {fa5 ,bp}, {siB4,n},
      {la4,c}, {fa4,c}, {sol4,c}, {re5,c}, {do5,n}, {silence,n}, 
      {FIN,n} 
    };

    // On joue la musique
    play(boot, indice);
  } 

  // Harry Potter
  if (indice == 2) {
    // Définition des constantes des durées de notes
    int c=273 ; int n =545; int np=818; int b =1091; int bp=1636;

    // Partition partielle de "Hedwig's Theme" ("Harry potter")
    int harry_melody[][2] = {
      {si3, n}, 
      {mi4, np}, {sol4, c}, {faD4, n}, 
      {mi4, b}, {si4, n}, 
      {la4, bp}, 
      {faD4, bp}, 
      {mi4, np}, {sol4, c}, {faD4, n}, 
      {reD4, b}, {fa4, n}, 
      {si3, bp}, 
      {silence, b}, {si3, n},

      {mi4, np}, {sol4, c}, {faD4, n}, 
      {mi4, b}, {si4, n},
      {re5, b}, {doD5, n}, 
      {do5, b}, {laB4, n},
      {do5, n}, {si4, n}, {siB4, n}, 
      {siB3, b}, {sol4, n},
      {mi4, bp},
      {silence, b}, {sol4, n},

      {si4, b}, {sol4, n},
      {si4, b}, {sol4, n}, 
      {do5, b}, {si4, n},
      {siB4, b}, {faD4, n}, 
      {sol4, np}, {si4, c}, {siB4, n},
      {siB3, b}, {si3, n}, 
      {si4, bp},
      {silence, b}, {sol4, n},

      {si4, b}, {sol4, n}, 
      {si4, b}, {sol4, n},
      {re5, b}, {doD5, n}, 
      {do5, b}, {solD5, n},
      {do5, n}, {si4, c}, {siB4, n}, 
      {siB3, b}, {sol4, n}, 
      {mi4, bp},  
      {silence, bp}, 

      {silence, n}, {FIN, n} // Fin du morceau
    };

    // On joue la musique
    play(harry_melody,indice);}

  // Tetris
  if (indice == 3) {
    // Définition des constantes des durées de notes
    int c=200; int n=400; int np=600; int b=800; int dc=100;

    // Partition partielle du thème principal de tetris
    int tetris[][2] = {
      {mi5,n}, {si4,c}, {do5,c}, {re5,c}, {mi5,dc}, {re5,dc}, {do5,c}, {si4,c},
      {la4,n}, {la4,c}, {do5,c}, {mi5,n}, {re5,c}, {do5,c},
      {si4,n}, {si4,dc}, {si4,dc}, {do5,c}, {re5,n}, {mi5,n},
      {do5,n}, {la4,n}, {la4,b}, 
      {re5,n}, {re5,c}, {fa5,c}, {la5,n}, {sol5,c}, {fa5,c},

      {mi5,n}, {mi5,c}, {do5,c}, {mi5,n}, {re5,c}, {do5,c}, 
      {si4,n}, {si4,dc}, {si4,dc}, {do5,c}, {re5,n}, {mi5,n}, 
      {do5,n}, {la4,n}, {la4,b},

      {silence,n}, 
      {FIN,0}
    };

    // On joue la musique
    play(tetris,indice);
  };
  // Musique Mystère
  if (indice == 4) {
    // Définition des constantes des durées de notes
    int a=600; int b=1000; int c=100; int d=400; int e=200; int f=300; int g=800; int s=30;

    // Partition partielle de la musique mystere "Never gonna give you up" ("Rick Astley")
    int mystere[][2] = {
      {doD5, a}, {miB5, b}, {silence, s}, {miB5, a}, {fa5, a}, {laB5, c}, {faD5, c}, {fa5, c}, {miB5, c}, {doD5, a}, {miB5, b}, {silence, d}, {laB4, e},{silence, s}, {laB4, b},
      {doD5, a}, {miB5, b}, {silence, s}, {miB5, a}, {fa5, a}, {laB5, c}, {faD5, c}, {fa5, c}, {miB5, c}, {doD5, a}, {miB5, b}, {silence, d}, {laB4, e},{silence, s}, {laB4, b},

      {silence, d}, {doD4, e}, {silence, s}, {doD4, e}, {silence, s}, {doD4, e}, {silence, s}, {doD4, e}, {miB4, d}, {silence, e}, {do4, e}, {siB3, e}, {laB3, b},
      {silence, e}, {siB3, e}, {silence, s}, {siB3, e}, {do4, e}, {silence, s}, {doD4, a}, {laB3, e}, {laB4, d}, {laB4, e}, {miB4, b},
      {silence, e}, {siB3, e}, {silence, s}, {siB3, e}, {do4, e}, {silence, s}, {doD4, e}, {siB3, e}, {doD4, e}, {miB4, d}, {silence, e}, {do4, e}, {siB3, e}, {silence, s}, {siB3, e}, {laB3, a},
      {silence, e}, {siB3, e}, {silence, s}, {siB3, e}, {do4, e}, {silence, s}, {doD4, d}, {laB3, e},{silence, s}, {laB3, e}, {miB4, e},{silence, s}, {miB4, e},{silence, s}, {miB4, e}, {fa4, e}, {miB4, g},
      {doD4, b}, {miB4, e}, {fa4, e}, {doD4, e}, {miB4, e}, {silence, s}, {miB4, e}, {silence, s}, {miB4, e}, {fa4, d}, {miB4, d}, {laB3, d},
      {silence, 100}, 
      {FIN, 0}
    };

    // On joue la musique
    play(mystere, indice);
    }
  
  // Marche Imperiale
  if (indice == 5) {
    // Définition des constantes des durées de notes
    int n=540; int np=810; int b=1080; int c=270; int dc=135; int cp=405;

    // Partition partielle de "La marche impériale" ("Star Wars")
    int vador[][2] = {
      {sol3,n},  {sol3,n},  {sol3,n},  {miB3,cp}, {siB3,dc},
      {sol3,n},  {miB3,cp}, {siB3,dc}, {sol3,b},
      {re4,n},   {re4,n},   {re4,n},   {miB4,cp}, {siB3,dc},
      {faD3,n}, {miB3,cp}, {siB3,dc}, {sol3,b},
      {sol4,n},  {sol3,cp}, {sol3,dc}, {sol4,n},  {faD4,cp}, {fa4,dc},

      {mi4,dc},  {miB4,dc}, {mi4,c},   {silence,c}, {laB3,c},  {reB4,n}, {do4,cp},  {si3,dc},
      {siB3,dc}, {la3,dc},  {siB3,c},  {silence,c}, {miB3,c},  {faD3,n}, {miB3,cp}, {faD3,dc},
      {siB3,n},  {sol3,cp}, {siB3,dc}, {re4,b},
      {sol4,n},  {sol3,cp}, {sol3,dc}, {sol4,n},    {faD4,cp}, {fa4,dc},

      {mi4,dc},  {miB4,dc}, {mi4,c},   {silence,c}, {laB3,c}, {reB4,n}, {do4,cp},  {si3,dc},
      {siB3,dc}, {la3,dc},  {siB3,c},  {silence,c}, {miB3,c}, {faD3,n}, {miB3,cp}, {siB3,dc},
      {sol3,n},  {miB3,cp}, {siB3,dc}, {sol3,b},
      {silence,n}, 
      {FIN,0}
    };

    // On joue la musique
    play(vador, indice);
  }
  // Partition partielle de "The Force" ("Star Wars")
  if (indice == 6) {
    // Définition des constantes des durées de notes
    int n=625; int np=937; int b=1250; int c=312; int cp=469; int dc=156; int c3=208;

    // Partition partielle de "The Force Thème" ("Star Wars")
    int theforce[][2] = {
      {sol3,n}, 
      {do4,b}, {re4,np}, {miB4,dc}, {fa4,dc},

      {miB4,b}, {sol3,np}, {sol3,c}, 
      {do4,np}, {re4,c}, {miB4,c}, {silence,c}, {miB4,c3}, {do4,c3}, {sol4,c3}, 
      {fa4,b}, {do4,n}, {silence,c}, {sol3,c},

      {do4,np}, {re4,c}, {miB4,cp}, {silence,dc}, {sol4,cp}, {miB4,dc},
      {do5,b}, {do4,n}, {miB4,c3}, {re4,c3}, {do4,c3},
      {sol4,n+c3}, {miB4,c3}, {do4,c3}, {sol3,cp}, {sol3,dc},

      {do4,b+n}, {silence,n}, 
      {FIN,0}
    };

    // On joue la musique
    play(theforce, indice);
  }
  // Mario
  if (indice == 7) {
    // Définition des constantes des durées de notes
    int n=333; int c=167; int n3=222; 

    // Partition partielle de "Ground Thème" ("Mario")
    int mario[][2] = {
      {mi4,c}, {mi4,c}, {silence,c}, {mi4,c}, {silence,c}, {do4,c}, {mi4,c},
      {sol4,n}, {silence,n}, {sol3,n}, {silence,n},
      {do4,n}, {silence,c}, {sol3,n}, {silence,c}, {mi3,n},
      {silence,c}, {la3,n}, {si3,n}, {siB3,c}, {la3,n},
      {sol3,n3}, {mi4,n3}, {sol4,n3}, {la4,n}, {fa4,c}, {sol4,c},
      {silence,c}, {mi4,n}, {do4,c}, {re4,c}, {si3,n}, {silence,c},

      {do2,n}, {sol4,c}, {faD4,c}, {fa4,c}, {reD4,c}, {do3,c}, {mi4,c},
      {fa2,c}, {solD3,c}, {la3,c}, {do4,c}, {do3,c}, {la3,c}, {do4,c}, {re4,c},
      {do2,n}, {sol4,c}, {faD4,c}, {fa4,c}, {reD4,c}, {sol2,c}, {mi4,c},
      {silence,c}, {do5,n}, {do5,c}, {do5,n}, {silence,n},
      {do2,n}, {sol4,c}, {faD4,c}, {fa4,c}, {reD4,c}, {do3,c}, {mi4,c},

      {fa2,c}, {solD3,c}, {la3,c}, {do4,c}, {do2,c}, {la3,c}, {do4,c}, {re4,c},
      {do2,n}, {miB4,n}, {silence,c}, {re4,n}, {silence,c},

      {do4,n},  {silence,n},  
      {FIN,0}
    };

    // On joue la musique
    play(mario, indice);
  };  
}