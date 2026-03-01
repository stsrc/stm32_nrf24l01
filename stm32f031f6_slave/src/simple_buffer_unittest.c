#include "simple_buffer.h"

#include <unity.h>

#include <string.h>


void setUp(void) {
  // runs before each test
}

void tearDown(void) {
  // runs after each test
}

void test_gps_like_data(void) {
  const char *message = "$GPGLL,5232.36020,N,02132.85787,E,084140.00,A,A*66\r\n";
  const char *result = "$GPGLL,5232.36020,N,02132.85787,E,084140.00,A,A*66";
  struct simple_buffer buffer;
  buffer_init(&buffer);
  const char *tmp = message;
  int8_t ret;
  while (*tmp != '\0') {
	ret = buffer_set_byte(&buffer, *tmp++);
	TEST_ASSERT_EQUAL(0, ret);
  }
  TEST_ASSERT_EQUAL(buffer_IsFull(&buffer), 0);
  char output[256] = {0};
  char data[256] = {0};
  const char *label = "$GPGLL,";
  ret = buffer_SearchGetLabel(&buffer, label, "\r\n", output, 0);
  TEST_ASSERT_EQUAL(ret, 0);
  memcpy(data, label, strlen(label));
  memcpy(data + strlen(label), output, strlen(buffer.memory));
  TEST_ASSERT_EQUAL(strcmp(data, result), 0);
}

void test_gps_like_data_with_distortion(void) {
  const char *message = "140.00,A,A*66\r\n$GPGLL,5232.36020,N,02132.85787,"
	  "E,084140.00,A,A*66\r\n$GPGLL,5232\r\n";
  const char *result = "$GPGLL,5232.36020,N,02132.85787,E,084140.00,A,A*66";
  struct simple_buffer buffer;
  buffer_init(&buffer);
  const char *tmp = message;
  int8_t ret;
  while (*tmp != '\0') {
	ret = buffer_set_byte(&buffer, *tmp++);
	TEST_ASSERT_EQUAL(0, ret);
  }
  TEST_ASSERT_EQUAL(buffer_IsFull(&buffer), 0);
  char output[256] = {0};
  char data[256] = {0};
  const char *label = "$GPGLL,";
  ret = buffer_SearchGetLabel(&buffer, label, "\r\n", output, 0);
  TEST_ASSERT_EQUAL(ret, 0);
  memcpy(data, label, strlen(label));
  memcpy(data + strlen(label), output, strlen(output));

  TEST_ASSERT_EQUAL(strcmp(data, result), 0);
}

void test_set_text(void) {
	const char *ut = "abcd";
	struct simple_buffer buffer;
	buffer_init(&buffer);
	int8_t ret = buffer_set_text(&buffer, ut, strlen(ut));
	TEST_ASSERT_EQUAL(ret, 0);
	uint8_t data;
	for (int i = 0; i < strlen(ut); i++) {
		ret = buffer_get_byte(&buffer, &data);
		TEST_ASSERT_EQUAL(ret, 0);
		TEST_ASSERT_EQUAL(data, ut[i]);
	}
	ret = buffer_set_text(&buffer, "1", 1);
	TEST_ASSERT_EQUAL(ret, 0);
	ret = buffer_get_byte(&buffer, &data);
	TEST_ASSERT_EQUAL(ret, 0);
	TEST_ASSERT_EQUAL(data, '1');
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_gps_like_data);
	RUN_TEST(test_gps_like_data_with_distortion);
	RUN_TEST(test_set_text);
	return UNITY_END();
}
