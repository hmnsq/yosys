#!/usr/bin/env python3

ਤਃ __future__ ਆਯਾਤ annotations
ਆਯਾਤ fileinput
ਆਯਾਤ json
ਤਃ pathlib ਆਯਾਤ ਰਸਤਾ

ਵਰਗ SimHelper:
    name: str = ""
    title: str = ""
    ports: str = ""
    ਸਰੋਤ: str = ""
    desc: list[str]
    ਕੋਡ: list[str]
    ਸਮੂਹ: str = ""
    ver: str = "1"
    ਟੈਗ_ਸੂਚੀ: list[str]

    ਪਰਿਭਾਸ਼ਾ __init__(self) -> ਕੁਝ_ਨਹੀਂ:
        self.desc = []
        self.ਟੈਗ_ਸੂਚੀ = []

    ਪਰਿਭਾਸ਼ਾ __str__(self) -> str:
        printed_fields = [
            "name", "title", "ports", "source", "desc", "code", "group", "ver",
            "tags",
        ]
        # generate C++ struct
        val = f"cell_help[{json.dumps(self.name)}] = "
        val += "{\n"
        ਲਈ ਖੇਤਰ ਵਿੱਚ printed_fields:
            ਖੇਤਰ_val = getattr(self, ਖੇਤਰ)
            ਜੇ isinstance(ਖੇਤਰ_val, list):
                ਖੇਤਰ_val = "\n".join(ਖੇਤਰ_val)
            ਖੇਤਰ_val = ਖੇਤਰ_val.strip()
            val += f'  {json.dumps(field_val)},\n'
        val += "};\n"
        ਵਾਪਸ val

ਪਰਿਭਾਸ਼ਾ simcells_reparse(cell: SimHelper):
    # cut manual signature
    cell.desc = cell.desc[3:]

    # code-block truth table
    new_desc = []
    indent = ""
    ਲਈ ਸਤਰ ਵਿੱਚ cell.desc:
        ਜੇ ਸਤਰ.startswith("Truth table:"):
            indent = "   "
            new_desc.ਕੱਢੋ()
            new_desc.extend(["::", ""])
        new_desc.append(indent + ਸਤਰ)
    cell.desc = new_desc

    # set version
    cell.ver = "2a"

simHelper = SimHelper()

ਲਈ ਸਤਰ ਵਿੱਚ fileinput.ਨਿਵੇਸ਼():
    ਸਤਰ = ਸਤਰ.rstrip()
    # special comments
    ਜੇ ਸਤਰ.startswith("//-"):
        simHelper.desc.append(ਸਤਰ[4:] ਜੇ ਲੰਬਾਈ(ਸਤਰ) > 4 ਨਹੀਂਤਾਂ "")
    ਜਾਂਜੇ ਸਤਰ.startswith("//* "):
        _, ਕੁੰਜੀ, val = ਸਤਰ.ਵੰਡੋ(maxsplit=2)
        setattr(simHelper, ਕੁੰਜੀ, val)

    # code parsing
    ਜੇ ਸਤਰ.startswith("module "):
        clean_ਸਤਰ = ਸਤਰ[7:].replace("\\", "").replace(";", "")
        simHelper.name, simHelper.ports = clean_ਸਤਰ.ਵੰਡੋ(maxsplit=1)
        simHelper.ਕੋਡ = []
        short_filename = ਰਸਤਾ(fileinput.filename()).name
        simHelper.ਸਰੋਤ = f'{short_filename}:{fileinput.filelineno()}'
    ਜਾਂਜੇ ਨਹੀਂ ਸਤਰ.startswith("endmodule"):
        ਸਤਰ = "    " + ਸਤਰ
    ਕੋਸ਼ਿਸ਼:
        simHelper.ਕੋਡ.append(ਸਤਰ.replace("\t", "    "))
    ਅਪਵਾਦ AttributeError:
        # no module definition, ignore line
        ਲੰਘੋ
    ਜੇ ਸਤਰ.startswith("endmodule"):
        short_filename = ਰਸਤਾ(fileinput.filename()).name
        ਜੇ simHelper.ver == "1" ਅਤੇ short_filename == "simcells.v":
            # default simcells parsing
            simcells_reparse(simHelper)

        # check help
        ਜੇ simHelper.desc ਅਤੇ simHelper.ver == "1" ਅਤੇ short_filename == "simlib.v" ਅਤੇ simHelper.desc[1].startswith('    '):
            simHelper.desc.ਕੱਢੋ(1)

        # check group
        ਪ੍ਰਤਿਜ੍ਞਾ simHelper.ਸਮੂਹ, f"techlibs/common/{simHelper.source}: {simHelper.name} cell missing group"

        # dump
        ਛਾਪੋ(simHelper)
        # new
        simHelper = SimHelper()
