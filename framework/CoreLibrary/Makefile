NAME=mscorlib

OUTPATH=../assemblies

CSC=csc.exe
MDP=../../tools/CanFly.Build.exe
FLAGS=-noconfig -target:library  -debug:full @coreAssembly.rsp
MDPFLAGS=

SRC= \
	CanFly/CanFlyMsg.cs \
	CanFly/CanFlyID.cs \
	CanFly/Color.cs \
	CanFly/Euler.cs \
	CanFly/Position.cs \
	CanFly/Runtime.cs \
	CanFly/Syscall.cs \
	CanFly/Extent.cs \
	CanFly/Pen.cs \
	CanFly/Point.cs \
	CanFly/Rect.cs \
	CanFly/Font.cs \
	CanFly/DirectoryEnumerator.cs \
	CanFly/Stream.cs \
	CanFly/Thread.cs \
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
	System/ApplicationException.cs \
	System/ArgumentException.cs \
	System/ArgumentNullException.cs \
	System/ArgumentOutOfRangeException.cs \
	System/Array.cs \
	System/AssemblyInfo.cs \
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
	System/Threading/Interlocked.cs \
	System/Threading/Monitor.cs \
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

all: $(OUTPATH)/$(NAME).pe

debug: MDPFLAGS += -d
debug: $(OUTPATH)/$(NAME).pe

release: $(OUTPATH)/$(NAME).pe
release: FLAGS += -optimize

$(OUTPATH)/$(NAME).pe : $(SRC)
	$(CSC) $(FLAGS) $(LIBS) $(SRC) -out:$(OUTPATH)/$(NAME).dll
	$(MDP) -c $(OUTPATH)/$(NAME).dll -p $(OUTPATH)/$(NAME).pe  $(MDPFLAGS) -m  -s ..\..\..\libs\ion\clr\mscorlib

clean:

fclean: clean
	$(RM) $(NAME)

re: fclean all