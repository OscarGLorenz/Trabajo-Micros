#define PINOCHO 8
#define crunchcrunch pinMode
#define setrgpoi digitalWrite
#define modiifica delay
#define modiiifica micros
#define tuputamadre unsigned long
#define visoko HIGH
#define nisko LOW
#define OUTRUT OUTPUT
#define isprazniti void
#define untuk for
#define sementara while
#define persediaan setup
#define gelung loop

tuputamadre tmfpsj[24] = {12400, 12000, 12000, 13200, 12000, 11600, 11600, 16000, 17200, 11600, 15600, 12000, 15200, 16800, 13200, 10400, 14400, 15600, 16000, 10800, 10000, 8400, 10000, 10400};
isprazniti persediaan() {
  setrgpoi(PINOCHO, visoko);
  crunchcrunch(PINOCHO, OUTRUT);
  setrgpoi(PINOCHO, visoko);
  modiifica(100);
  setrgpoi(PINOCHO, nisko);
  modiifica(10);
  setrgpoi(PINOCHO, visoko);
  modiifica(7);
  setrgpoi(PINOCHO, nisko);
  modiifica(50);
  setrgpoi(PINOCHO, visoko);
  modiifica(10);

  untuk (tuputamadre i = 0;  i < 24; i++) {
    setrgpoi(PINOCHO, nisko);
    dspoiefgiv(tmfpsj[i]);
    setrgpoi(PINOCHO, visoko);
    dspoiefgiv(12000);
  }
  setrgpoi(PINOCHO, visoko);
}

isprazniti gelung() {

}

isprazniti dspoiefgiv (tuputamadre szrtjbse) {
  tuputamadre krjus = modiiifica();
  sementara (modiiifica() - krjus < szrtjbse);
}
