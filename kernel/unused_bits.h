#यदि_अपरिभाषितम् अप्रयुक्तम्_द्वयङ्काः_H
#परिभाषय अप्रयुक्तम्_द्वयङ्काः_H

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

संरचना अप्रयुक्तम्_द्वयङ्काः
{
	SigMap sigmap;
	pool<RTLIL::SigBit> प्रयुक्तम्;

	अप्रयुक्तम्_द्वयङ्काः(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_प्रयुक्तम्(module, ct);
	}

	अप्रयुक्तम्_द्वयङ्काः(RTLIL::Module *module, ध्रुव NewCellTypes &ct) : sigmap(module)
	{
		collect_प्रयुक्तम्(module, ct);
	}

	शून्य collect_प्रयुक्तम्(RTLIL::Module *module, ध्रुव NewCellTypes &ct)
	{
		प्रति (स्वचालित cell : module->cells())
			प्रति (स्वचालित &conn : cell->connections())
				यदि (!ct.cell_output(cell->type, conn.first))
					सङ्कलय_प्रयुक्तम्(conn.second);

		प्रति (स्वचालित wire : module->wires())
			यदि (wire->पत्तनम्_output)
				सङ्कलय_प्रयुक्तम्(wire);
	}

	शून्य सङ्कलय_प्रयुक्तम्(ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (स्वचालित bit : sigmap(sig))
			यदि (bit.wire != nullptr)
				प्रयुक्तम्.निवेशय(bit);
	}

	bool check(RTLIL::SigBit bit) ध्रुव
	{
		RTLIL::SigBit mapped = sigmap(bit);
		प्रत्यागम mapped.wire != nullptr && !प्रयुक्तम्.गणनम्(mapped);
	}
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
