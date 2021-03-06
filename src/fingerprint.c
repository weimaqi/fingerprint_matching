#include "fingerprint.h"

Fingerprint fingerprint_create_empty() {
  Fingerprint fp = {0, 0, NULL, minutiarawset_create_empty(), NULL,
                    secondaryfeatureset_create_empty()};
  return fp;
}

static void stripextension(const char * const str, char buffer[1024]) {
  size_t i, j = 0;
  for (i = 0 ; i < strlen(str) ; ++i) {
    if (str[i] >= 48 && str[i] <= 57) {
      buffer[j] = str[i];
      j ++;
    } else {
      continue;
    }
  }
}

Fingerprint fingerprint_load(const char *const filepath, bool *success) {
  MinutiaRawSet rawset = minutiarawset_load(filepath, success);
  Fingerprint fp;
  char filename[1024] = {'\0'};
  stripextension(filepath, filename);

  if (success) {
    int i;
    fp.id = 0; // TODO compute its id with hash
    fp.name = calloc(strlen(filename)+1, sizeof(char));
    memset(fp.name, '\0', strlen(filename)+1);
    strcpy(fp.name, filename);
    fp.rawset = rawset;
    fp.numset = rawset.size;
    fp.mset = (MinutiaSet *) calloc(fp.numset, sizeof(MinutiaSet));
    for (i = 0 ; i < rawset.size ; ++i) {
      fp.mset[i] = minutiaset_create(rawset, i);
    }
    fp.sset = secondaryfeatureset_create(fp.rawset);
    return fp;
  }

  fp = fingerprint_create_empty();
  return fp;
}

int fingerprint_get_num_matched_minutia(const Fingerprint query,
                                        const Fingerprint reference) {
  int count = 0, i, j;
  for (i = 0 ; i < reference.numset ; ++i) {
    for (j = 0 ; j < query.numset ; ++j) {
      count += minutiaset_compute_num_matched(query.mset[j], reference.mset[i]);
    }
  }
  return count;
}

int fingerprint_get_num_matched_secondary_feature(const Fingerprint query,
                                                  const Fingerprint reference) {
  return secondaryfeatureset_get_num_matched(query.sset, reference.sset);
}

int fingerprint_get_n(const Fingerprint query, const Fingerprint reference) {
  return fingerprint_get_num_matched_minutia(query, reference) +
      fingerprint_get_num_matched_secondary_feature(query, reference);
  /** return fingerprint_get_num_matched_secondary_feature(query, reference); */
  /** return fingerprint_get_num_matched_minutia(query, reference); */
}

int fingerprint_get_num_minutia(const Fingerprint fp) {
  return fp.rawset.size;
}

SecondaryFeature fingerprint_get_center_secondary_feature(Fingerprint fp) {
  MinutiaeRaw center, n1, n2;
  minutiarawset_get_mean(fp.rawset, &center);
  minutiarawset_get_2_closest(fp.rawset, center, &n1, &n2);

  SecondaryFeature sf = secondaryfeature_create(center, n1, n2);
  return sf;
}

bool fingerprint_match_secondary_feature(const Fingerprint fp1,
                                         const Fingerprint fp2) {
  const SecondaryFeature sf1 = fingerprint_get_center_secondary_feature(fp1);
  const SecondaryFeature sf2 = fingerprint_get_center_secondary_feature(fp2);

  return secondaryfeature_is_matched(sf1, sf2);
}

void fingerprint_info(const Fingerprint fp) {
#ifdef DEBUG
  if (fp.name != NULL)
    printf("name of fingerprint: %s\n", fp.name);
#endif

  minutiarawset_info(fp.rawset);

  int i;
  for (i = 0 ; i < fp.numset ; ++i) {
#ifdef DEBUG
    printf("minutia set %d:\n", i);
#endif
    minutiaset_info(fp.mset[i]);
  }
}

void fingerprint_release(Fingerprint fp) {
  int i;
  if (fp.name) free(fp.name);

  minutiarawset_release(fp.rawset);
  for (i = 0 ; i < fp.numset ; ++i) {
    minutiaset_release(fp.mset[i]);
  }
  secondaryfeatureset_release(fp.sset);
}
