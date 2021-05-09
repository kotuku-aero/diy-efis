//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System.Collections;

namespace System.Text.RegularExpressions
{
    [Serializable]
    internal class MatchEnumerator : IEnumerator
    {
        #region Fields
        
        internal int _curindex;
        internal bool _done;
        internal Match _match;
        internal MatchCollection _matchcoll;

        #endregion

        #region Properties

        public object Current
        {
            get
            {
                if (_match == null)
                {
                    throw new InvalidOperationException("EnumNotStarted");
                }

                return _match;
            }
        }

        #endregion

        #region Methods

        internal MatchEnumerator(MatchCollection matchcoll)
        {
            _matchcoll = matchcoll;
        }

        /// <inheritdoc/>
        public bool MoveNext()
        {
            if (_done)
            {
                return false;
            }

            _match = _matchcoll.GetMatch(_curindex++);
            if (_match == null)
            {
                _done = true;
                return false;
            }

            return true;
        }

        /// <inheritdoc/>
        public void Reset()
        {
            _curindex = 0;
            _done = false;
            _match = null;
        }

        #endregion
    }
}