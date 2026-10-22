#!/usr/bin/env python3

तः __future__ आनयन annotations
आनयन fileinput
आनयन json
तः pathlib आनयन मार्गः

वर्ग SimHelper:
    name: str = ""
    title: str = ""
    ports: str = ""
    source: str = ""
    desc: list[str]
    संहिता: list[str]
    गणः: str = ""
    ver: str = "1"
    चिह्नकानि: list[str]

    परिभाषा __init__(self) -> किमपि_न:
        self.desc = []
        self.चिह्नकानि = []

    परिभाषा __str__(self) -> str:
        printed_fields = [
            "name", "title", "ports", "source", "desc", "code", "group", "ver",
            "tags",
        ]
        # generate C++ struct
        val = f"cell_help[{json.dumps(self.name)}] = "
        val += "{\n"
        प्रति क्षेत्रम् अन्तः printed_fields:
            क्षेत्रम्_val = getattr(self, क्षेत्रम्)
            यदि isinstance(क्षेत्रम्_val, list):
                क्षेत्रम्_val = "\n".join(क्षेत्रम्_val)
            क्षेत्रम्_val = क्षेत्रम्_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        प्रत्यागम val

परिभाषा simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    प्रति पङ्क्तिः अन्तः cell.desc:
        यदि पङ्क्तिः.startswith("Truth table:"):
            indent = "   "
            new_desc.निष्कासय()
            new_desc.extend(["::", ""])
        new_desc.append(indent + पङ्क्तिः)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

प्रति पङ्क्तिः अन्तः fileinput.निवेशः():
    पङ्क्तिः = पङ्क्तिः.rstrip()
    # special comments
    यदि पङ्क्तिः.startswith("//-"):
        simHelper.desc.append(पङ्क्तिः[4:] यदि दीर्घता(पङ्क्तिः) > 4 अन्यथा "")
    नोचेद्यदि पङ्क्तिः.startswith("//* "):
        _, कुञ्चिका, val = पङ्क्तिः.विभज(maxsplit=2)
        setattr(simHelper, कुञ्चिका, val)

    # code parsing
    यदि पङ्क्तिः.startswith("module "):
        clean_पङ्क्तिः = पङ्क्तिः[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_पङ्क्तिः.विभज(maxsplit=1)
        simHelper.संहिता = []
        short_filename = मार्गः(fileinput.filename()).name
        simHelper.source = f'{short_filename}:{fileinput.filelineno()}'
    नोचेद्यदि न पङ्क्तिः.startswith("endmodule"):
        पङ्क्तिः = "    " + पङ्क्तिः
    यत्न:
        simHelper.संहिता.append(पङ्क्तिः.replace("\t", "    "))
    अपवाद AttributeError:
        # no module definition, ignore line
        अतिक्रम
    यदि पङ्क्तिः.startswith("endmodule"):
        short_filename = मार्गः(fileinput.filename()).name
        यदि simHelper.ver == "1" च short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        यदि simHelper.desc च simHelper.ver == "1" च short_filename == "simlib.v" च simHelper.desc[1].startswith('    '):
            simHelper.desc.निष्कासय(1)

        # check group
        प्रतिज्ञा simHelper.गणः, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        मुद्रय(simHelper)
        # new
        simHelper = SimHelper()
