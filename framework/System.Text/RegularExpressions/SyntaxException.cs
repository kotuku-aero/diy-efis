﻿/*
* Licensed to the Apache Software Foundation (ASF) under one or more
* contributor license agreements.  See the NOTICE file distributed with
* this work for additional information regarding copyright ownership.
* The ASF licenses this file to You under the Apache License, Version 2.0
* (the "License"); you may not use this file except in compliance with
* the License.  You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
* 
* Ported to C# for the .Net Micro Framework by <a href="mailto:juliusfriedman@gmail.com">Julius Friedman</a>
* http://netmf.codeplex.com/
* 
* Exception thrown to indicate a syntax error in a regular expression.
* This is a non-checked exception because you should only have problems compiling
* a regular expression during development.
* If you are making regular expresion programs dynamically then you can catch it
* if you wish. But should not be forced to.
*
* @author <a href="mailto:jonl@muppetlabs.com">Jonathan Locke</a>
* @author <a href="mailto:gholam@xtra.co.nz>Michael McCallum</a>
* @version $Id: RESyntaxException.java 518156 2007-03-14 14:31:26Z vgritsenko $
*
* Adjusted by Laurent Ellerbach laurent@ellerbach.net
* 2021/03/13
*/

namespace System.Text.RegularExpressions
{

    /// <summary>
    /// Exception thrown to indicate a syntax error in a regular expression.
    /// This is a non-checked exception because you should only have problems compiling
    /// a regular expression during development.
    /// If you are making regular expresion programs dynamically then you can catch it
    /// if you wish. But should not be forced to.
    /// </summary>
    public class RegexpSyntaxException : Exception
    {
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="s">Further description of the syntax error</param>
        public RegexpSyntaxException(string s)
            : base("Syntax error: " + s)
        {
        }
    }

}
