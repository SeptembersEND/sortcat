#include "src/lib.c"
int main(void) {
  printf("[+] Started %ld\n", time(NULL));
  Image img = import("in.ppm");
  //Image buf = sobelOperator(&img);
  //quicksort(&img);

  char *text = "All human beings are born free and equal in dignity and "
               "rights. They are endowed with reason and conscience and should "
               "act towards one another in a spirit of brotherhood.";
  stenography_put(&img, 4, (uint8_t*)text, strlen((const char*)text));
  uint8_t* rtxt = stenography_get(&img, 4);
  printf("msg> '%s'\n", rtxt);

  ende = 1;
  snapshot(&img);
}

// vim: shiftwidth=2 tabstop=2 expandtab
