using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace CanFly
{
  class ParameterNode : TreeNode, IPropertyBag
  {
    private IParameterDefinition defn;
    public ParameterNode(TreeNode parent, IParameterDefinition defn)
      : base(defn.Name)
    {
      parent.TreeView.Invoke((MethodInvoker) delegate { parent.Nodes.Add(this); });
      // add to children

      this.defn = defn;
    }

    public object PropertyBag { get { return defn; } }
  }
}
