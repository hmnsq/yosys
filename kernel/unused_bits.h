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
		प्रति (auto cell : module->cells())
			प्रति (auto &conn : cell->connections())
				यदि (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		प्रति (auto wire : module->wires())
			यदि (wire->port_output)
				add_used(wire);
	}

	शून्य add_used(ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (auto bit : sigmap(sig))
			यदि (bit.wire != nullptr)
				used.जोड़ो(bit);
	}

	bool check(RTLIL::SigBit bit) ध्रुव
	{
		RTLIL::SigBit mapped = sigmap(bit);
		प्रत्यागम mapped.wire != nullptr && !used.गिनती(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
