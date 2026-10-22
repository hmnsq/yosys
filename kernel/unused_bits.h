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

	UnusedBits(RTLIL::Module *module, ध्रुव NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	शून्य collect_used(RTLIL::Module *module, ध्रुव NewCellTypes &ct)
	{
		अत्थाय (auto cell : module->cells())
			अत्थाय (auto &conn : cell->connections())
				सचे (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		अत्थाय (auto wire : module->wires())
			सचे (wire->port_output)
				add_used(wire);
	}

	शून्य add_used(ध्रुव RTLIL::SigSpec &sig)
	{
		अत्थाय (auto bit : sigmap(sig))
			सचे (bit.wire != nullptr)
				used.निवेशय(bit);
	}

	bool check(RTLIL::SigBit bit) ध्रुव
	{
		RTLIL::SigBit mapped = sigmap(bit);
		पटिनिवत्त mapped.wire != nullptr && !used.गणनम्(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
