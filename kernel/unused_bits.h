#अगर_अघोषित UNUSED_BITS_H
#घोषणा UNUSED_BITS_H

#समावेश "kernel/yosys.h"
#समावेश "kernel/sigtools.h"
#समावेश "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

काष्ठा UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, स्थिर NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	व्योम collect_used(RTLIL::Module *module, स्थिर NewCellTypes &ct)
	{
		क्रम (स्वतः cell : module->cells())
			क्रम (स्वतः &conn : cell->connections())
				अगर (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		क्रम (स्वतः wire : module->wires())
			अगर (wire->port_output)
				add_used(wire);
	}

	व्योम add_used(स्थिर RTLIL::SigSpec &sig)
	{
		क्रम (स्वतः bit : sigmap(sig))
			अगर (bit.wire != nullptr)
				used.जोड़ो(bit);
	}

	bool check(RTLIL::SigBit bit) स्थिर
	{
		RTLIL::SigBit mapped = sigmap(bit);
		वापस mapped.wire != nullptr && !used.गिनती(mapped);
	}
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
