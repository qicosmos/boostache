#include <string>
#include <iostream>
#include <fstream>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "test_template_fixture.hpp"

// Tests that a simple mustache tag is replaced
BOOST_FIXTURE_TEST_CASE(TestSectionsMustacheFromString, TemplateFixture)
{
	template_string = "Hi I am {{name}}.\n";
	template_string += "{{# showme}}";
	template_string += "I like {{pet}}.";
	template_string += "{{/ showme}}";
	template_string += "{{# dontshowme}}";
	template_string += "If you see this, something went wrong.";
	template_string += "{{/ dontshowme}}";

	set_tag_value("name", "Daniel");
	set_tag_value("pet", "turtles");
	set_tag_value("showme", "true");
	set_tag_value("dontshowme", "false");

	generate_template();

	std::string expected = "Hi I am Daniel.\n";
	expected += "I like turtles.";
	// TODO: BOOST_CHECK_EQUAL(expected, result);
}

