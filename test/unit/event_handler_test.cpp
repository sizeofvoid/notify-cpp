/*
 * Copyright (c) 2019 Rafael Sadowski <rafael@sizeofvoid.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <notify-cpp/event.h>

#include <boost/test/unit_test.hpp>

using namespace notifycpp;

BOOST_AUTO_TEST_CASE(EventOperatorTest)
{
    BOOST_CHECK_EQUAL((Event::all & Event::close_write), Event::close_write);
    BOOST_CHECK_EQUAL((Event::close & Event::close_write), Event::close_write);
    BOOST_CHECK_EQUAL((Event::all & Event::close), Event::close);
    BOOST_CHECK_EQUAL((Event::all & Event::access | Event::modify), Event::access | Event::modify);
    BOOST_CHECK_EQUAL((Event::all & Event::moved_from), Event::moved_from);
    BOOST_CHECK_EQUAL((Event::move & Event::moved_from), Event::moved_from);
    BOOST_CHECK(!((Event::move & Event::open) == Event::open));
}

BOOST_AUTO_TEST_CASE(EventToStringTest)
{
    BOOST_CHECK_EQUAL(toString(Event::all), std::string("access,modify,attrib,close_write,close_nowrite,open,moved_from,moved_to,create,delete,delete_self,move_self,close,move,all"));

    BOOST_CHECK_EQUAL(toString(Event::access), std::string("access"));
    BOOST_CHECK_EQUAL(toString(Event::access | Event::close_nowrite), std::string("access,close_nowrite"));
    BOOST_CHECK_EQUAL(toString(Event::close_nowrite | Event::access), std::string("access,close_nowrite"));
}
