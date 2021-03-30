//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

using System.Collections.Generic;

namespace CanFly.Tools.MetadataProcessor.Core
{
    public class MethodDef
    {
        public string ReferenceId;

        public string Flags;

        public string Implementation;

        public string RVA;

        public string Name;

        public string Signature;

        public string Locals;

        public List<ExceptionHandler> ExceptionHandlers = new List<ExceptionHandler>();

        public string ILCodeInstructionsCount;

        public List<ILCode> ILCode = new List<ILCode>();
    }
}
