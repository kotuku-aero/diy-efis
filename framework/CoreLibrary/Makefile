NAME=mscorlib

CSC=csc.exe
MDP=CanFly.Build.exe
FLAGS=-noconfig -target:library @coreAssembly.rsp
MDPFLAGS=-d $(NAME).pdb

SRC= \
	CanFly/Runtime.cs \
    CanFly/Syscall.cs \
    System/Collections/ArrayList.cs \
    System/Collections/ICollection.cs \
    System/Collections/IComparer.cs \
    System/Collections/IEnumerable.cs \
    System/Collections/IEnumerator.cs \
    System/Collections/IEqualityComparer.cs \
    System/Collections/IList.cs \
    System/ComponentModel/EditorBrowsableAttribute.cs \
    System/Diagnostics/ConditionalAttribute.cs \
    System/Diagnostics/Debug.cs \
    System/Diagnostics/Debugger.cs \
    System/Diagnostics/DebuggerAttributes.cs \
    System/Globalization/DateTimeFormat.cs \
    System/Globalization/DateTimeFormatInfo.cs \
    System/Globalization/NumberFormatInfo.cs \
    System/IO/IOException.cs \
    System/IO/SeekOrigin.cs \
    System/IO/Stream.cs \
    System/Reflection/AssemblyAttributes.cs \
    System/Reflection/AssemblyReflectionAttributes.cs \
    System/Reflection/DefaultMemberAttribute.cs \
    System/Reflection/FieldReflectionAttributes.cs \
    System/Reflection/MethodImplAttributes.cs \
    System/Runtime/CompilerServices/AccessedThroughPropertyAttribute.cs \
    System/Runtime/CompilerServices/ExtensionAttribute.cs \
    System/Runtime/CompilerServices/IndexerNameAttribute.cs \
    System/Runtime/CompilerServices/InternalsVisibleToAttribute.cs \
    System/Runtime/CompilerServices/MethodImplAttribute.cs \
    System/Runtime/InteropServices/Attributes.cs \
    System/Runtime/InteropServices/CharSet.cs \
    System/Runtime/InteropServices/LayoutKind.cs \
    System/AppDomainUnloadedException.cs \
    System/ApplicationException.cs \
    System/ArgumentException.cs \
    System/ArgumentNullException.cs \
    System/ArgumentOutOfRangeException.cs \
    System/Array.cs \
    System/AssemblyInfo.cs \
    System/AsyncCallback.cs \
    System/Attribute.cs \
    System/AttributeTargets.cs \
    System/AttributeUsageAttribute.cs \
    System/BitConverter.cs \
    System/Boolean.cs \
    System/Byte.cs \
    System/Char.cs \
    System/CLSCompliantAttribute.cs \
    System/Convert.cs \
    System/DateTime.cs \
    System/DayOfWeek.cs \
    System/DBNull.cs \
    System/Decimal.cs \
    System/Delegate.cs \
    System/Double.cs \
    System/Enum.cs \
    System/EventArgs.cs \
    System/EventHandler.cs \
    System/Exception.cs \
    System/FlagsAttribute.cs \
    System/Guid.cs \
    System/IAsyncResult.cs \
    System/ICloneable.cs \
    System/IComparable.cs \
    System/ICustomFormatter.cs \
    System/IDisposable.cs \
    System/IFormatProvider.cs \
    System/IFormattable.cs \
    System/IndexOutOfRangeException.cs \
    System/Int16.cs \
    System/Int32.cs \
    System/Int64.cs \
    System/IntPtr.cs \
    System/InvalidCastException.cs \
    System/InvalidOperationException.cs \
    System/MarshalByRefObject.cs \
    System/Math.cs \
    System/MulticastDelegate.cs \
    System/NonSerializedAttribute.cs \
    System/NotImplementedException.cs \
    System/NotSupportedException.cs \
    System/NullReferenceException.cs \
    System/Number.cs \
    System/Object.cs \
    System/ObjectDisposedException.cs \
    System/ObsoleteAttribute.cs \
    System/OutOfMemoryException.cs \
    System/ParamArrayAttribute.cs \
    System/Random.cs \
    System/RuntimeArgumentHandle.cs \
    System/RuntimeFieldHandle.cs \
    System/RuntimeMethodHandle.cs \
    System/RuntimeTypeHandle.cs \
    System/SByte.cs \
    System/SerializableAttribute.cs \
    System/Single.cs \
    System/SpanByte.cs \
    System/String.cs \
    System/SystemException.cs \
    System/TargetFrameworkAttribute.cs \
    System/ThreadAttributes.cs \
    System/Threading\Interlocked.cs \
    System/TimeSpan.cs \
    System/TypeCode.cs \
    System/TypedReference.cs \
    System/UInt16.cs \
    System/UInt32.cs \
    System/UInt64.cs \
    System/UIntPtr.cs \
    System/ValueType.cs \
    System/Version.cs \
    System/Void.cs \
    System/WeakReference.cs

LIBS=
RM =rm -rf

OUTPATH=Debug

all: $(NAME)

$(NAME) : $(SRC)
	$(CSC) $(FLAGS) $(LIBS) $(SRC) -out:$(OUTPATH)/$(NAME).dll
#	$(MDP) -c $(OUTPATH)/$(NAME).dll -o $(OUTPATH)/$(NAME).pe

clean:

fclean: clean
	$(RM) $(NAME)

re: fclean all