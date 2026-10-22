#ifndef UNUSED_BITS_H
#define UNUSED_BITS_H

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

संरचना UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, अचर NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	शून्य collect_used(RTLIL::Module *module, अचर NewCellTypes &ct)
	{
		लागि (स्वतः cell : module->cells())
			लागि (स्वतः &conn : cell->connections())
				यदि (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		लागि (स्वतः wire : module->wires())
			यदि (wire->port_output)
				add_used(wire);
	}

	शून्य add_used(अचर RTLIL::SigSpec &sig)
	{
		लागि (स्वतः bit : sigmap(sig))
			यदि (bit.wire != nullptr)
				used.घुसाउ(bit);
	}

	bool check(RTLIL::SigBit bit) अचर
	{
		RTLIL::SigBit mapped = sigmap(bit);
		फर्काऊ mapped.wire != nullptr && !used.गन्ती(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
