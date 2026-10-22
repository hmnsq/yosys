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

	UnusedBits(RTLIL::Module *module, स्थिरांक NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	शून्य collect_used(RTLIL::Module *module, स्थिरांक NewCellTypes &ct)
	{
		साठी (स्वतः cell : module->cells())
			साठी (स्वतः &conn : cell->connections())
				जर (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		साठी (स्वतः wire : module->wires())
			जर (wire->port_output)
				add_used(wire);
	}

	शून्य add_used(स्थिरांक RTLIL::SigSpec &sig)
	{
		साठी (स्वतः bit : sigmap(sig))
			जर (bit.wire != nullptr)
				used.घाला(bit);
	}

	bool check(RTLIL::SigBit bit) स्थिरांक
	{
		RTLIL::SigBit mapped = sigmap(bit);
		परत mapped.wire != nullptr && !used.मोजणी(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
