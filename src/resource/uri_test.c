#include <stdlib.h>
#include "uri.h"
#include "../assertion.h"
#include "../error.h"

void testParseInvalidUri() {
    char input[] = "dragon.mtl";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void testParseFileScheme() {
    char input[] = "file:///path/to/file.ext";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, STATUS_OK);

    assertStringsEqual(uri.scheme, "file");
    assertStringsEqual(uri.host, "");
    assertStringsEqual(uri.path, "/path/to/file.ext");
}

void testParseFileScheme2() {
    char input[] = "file:///config.json";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, STATUS_OK);

    assertStringsEqual(uri.scheme, "file");
    assertStringsEqual(uri.host, "");
    assertStringsEqual(uri.path, "/config.json");
}

void testParseFileScheme3() {
    char input[] = "file:/config.json";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, STATUS_OK);

    assertStringsEqual(uri.scheme, "file");
    assertPointersEqual(uri.host, NULL);
    assertStringsEqual(uri.path, "/config.json");
}

void testParseURI() {
    char input[] = "https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, STATUS_OK);

    assertStringsEqual(uri.scheme, "https");
    assertStringsEqual(uri.username, "john.doe");
    assertIsNull(uri.password);
    assertStringsEqual(uri.host, "www.example.com");
    assertStringsEqual(uri.port, "123");
    assertStringsEqual(uri.host, "www.example.com");
    assertStringsEqual(uri.path, "/forum/questions/");
    assertStringsEqual(uri.query, "tag=networking&order=newest");
    assertStringsEqual(uri.fragment, "top");
}

void testUriToStr() {
    char input[] = "https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top";
    char *expected = input;
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, STATUS_OK);

    char *actual = uriToStr(&uri);
    assertStringsEqual(actual, expected);
}

void testUriSwapExt() {
    char input[] = "https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top";
    char *expected = "https://john.doe@www.example.com:123/forum/questions/.txt?tag=networking&order=newest#top";
    struct URI uri;
    int result = parseURI(&uri, input);

    assertIntegersEqual(result, STATUS_OK);

    char *actual = uriSwapExt(&uri, "txt", 0);
    assertStringsEqual(actual, expected);
    free(actual);
}

void testUriSwapFile() {
    char input[] = "https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top";
    char *expected = "https://john.doe@www.example.com:123/forum/questions/dragon.mtl?tag=networking&order=newest#top";
    struct URI uri;
    int result = parseURI(&uri, input);

    assertIntegersEqual(result, STATUS_OK);

    char *actual = uriSwapFile(&uri, "dragon.mtl", 0);
    assertStringsEqual(actual, expected);
    free(actual);
}

void testUriSwapFile2() {
    char input[] = "file:/data/dragon/dragon.obj";
    char *expected = "file:/data/dragon/dragon.mtl";
    struct URI uri;
    int result = parseURI(&uri, input);

    assertIntegersEqual(result, STATUS_OK);

    char *actual = uriSwapFile(&uri, "dragon.mtl", 0);
    assertStringsEqual(actual, expected);
    free(actual);
}

void testParseURINoQueryWithFragment() {
    char input[] = "file:/data/city/concrete_barrier.obj#Barrier";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, STATUS_OK);
    assertStringsEqual(uri.scheme, "file");
    assertIsNull(uri.host);
    assertStringsEqual(uri.path, "/data/city/concrete_barrier.obj");
    assertIsNull(uri.query);
    assertStringsEqual(uri.fragment, "Barrier");

    uriRelease(&uri);
}

void testParseURINoQueryWithFragment2() {
    char input[] = "file:/data/vending_machine/vending_machine.obj#";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, STATUS_OK);
    assertStringsEqual(uri.scheme, "file");
    assertIsNull(uri.host);
    assertStringsEqual(uri.path, "/data/vending_machine/vending_machine.obj");
    assertIsNull(uri.query);
    assertStringsEqual(uri.fragment, "");

    uriRelease(&uri);
}

void testURIStrip() {
    char input[] = "file://user:pass@host:port/a/path/?query1=value1&query2=value2#fragment";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, STATUS_OK);

    // Test strippng the scheme.
    char *output = uriStrip(&uri, URI_SCHEME);
    assertStringsEqual(output, "//user:pass@host:port/a/path/?query1=value1&query2=value2#fragment");
    free(output);
    // Test strippng the username.
    output = uriStrip(&uri, URI_USERNAME);
    assertStringsEqual(output, "file://:pass@host:port/a/path/?query1=value1&query2=value2#fragment");
    free(output);
    // Test strippng the password.
    output = uriStrip(&uri, URI_PASSWORD);
    assertStringsEqual(output, "file://user@host:port/a/path/?query1=value1&query2=value2#fragment");
    free(output);
    // Test strippng the host.
    output = uriStrip(&uri, URI_HOST);
    assertStringsEqual(output, "file://user:pass@:port/a/path/?query1=value1&query2=value2#fragment");
    free(output);
    // Test strippng the port.
    output = uriStrip(&uri, URI_PORT);
    assertStringsEqual(output, "file://user:pass@host/a/path/?query1=value1&query2=value2#fragment");
    free(output);
    // Test strippng the path.
    output = uriStrip(&uri, URI_PATH);
    assertStringsEqual(output, "file://user:pass@host:port?query1=value1&query2=value2#fragment");
    free(output);
    // Test strippng the query.
    output = uriStrip(&uri, URI_QUERY);
    assertStringsEqual(output, "file://user:pass@host:port/a/path/#fragment");
    free(output);
    // Test strippng the fragment.
    output = uriStrip(&uri, URI_FRAGMENT);
    assertStringsEqual(output, "file://user:pass@host:port/a/path/?query1=value1&query2=value2");
    free(output);

    uriRelease(&uri);
}

void testParseURICollapseParentDirectory() {
    char input[] = "file:/data/city/../ground/dirt.png#";
    struct URI uri;
    int result = parseURI(&uri, input);
    assertIntegersEqual(result, STATUS_OK);
    assertStringsEqual(uri.scheme, "file");
    assertIsNull(uri.host);
    assertStringsEqual(uri.path, "/data/ground/dirt.png");
    assertIsNull(uri.query);
    assertStringsEqual(uri.fragment, "");

    uriRelease(&uri);
}

void uriTest() {
    testParseInvalidUri();
    testParseFileScheme();
    testParseFileScheme2();
    testParseFileScheme3();
    testParseURI();
    testUriToStr();
    testUriSwapExt();
    testUriSwapFile();
    testUriSwapFile2();
    testParseURINoQueryWithFragment();
    testParseURINoQueryWithFragment2();
    testURIStrip();
    testParseURICollapseParentDirectory();
}
