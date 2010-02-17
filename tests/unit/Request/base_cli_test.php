<?php
require_once BASEPATH . '/libpp/lib/Request/classes.inc';

class PXRequestBaseCLITest extends UnitTestCase {

    private static $original_bucks_server;

    function setUp() {
        self::$original_bucks_server = $_SERVER;
        self::setUp_cli_env();
    }
    
    /* restore original environment after each test */
    function tearDown() {
        $_SERVER = self::$original_bucks_server;
    }

    function test_Instance_Should_be_PXRequestBase() {
        $request = new PXRequest();
        $this->assertIsA($request, 'PXRequest');
    }

    
    function test_default_HTTPMethod_should_be_CLI() {
        $this->assertEqual(PXRequest::GetHttpMethod(), 'CLI');
    }
    
    function test_default_HTTPReferer_should_be_NULL() {
        $this->assertNull(PXRequest::getHttpReferer());
    }
    
    //TODO: why should we want two referer functions?
    function test_default_Referer_should_be_NULL() {
        $request = new PXRequest();
        $this->assertNull($request->getReferer());
    }

    function test_default_HTTPRequestURI_should_be_NULL() {
        $this->assertNull(PXRequest::getRequestUri());
    }

    private static function setUp_cli_env() {
        $_SERVER['REQUEST_METHOD'] = NULL;
        $_SERVER['HTTP_REFERER']   = NULL;
        $_SERVER['REQUEST_URI']    = NULL;
        $_SERVER['SCRIPT_NAME']    = NULL;
        $_SERVER['HTTP_X_HOST']    = NULL;
        $_SERVER['HTTP_HOST']      = NULL;
    }

}

?>
